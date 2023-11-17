#include "session.h"
#include "utils.h"
//-----------------------------------------------------------------------------
Session::Session(boost::asio::io_service& ios)
    : m_Socket(ios)
{
    m_Data.resize(BUFFER_SIZE);
}
//-----------------------------------------------------------------------------
Session::~Session()
{

}
//-----------------------------------------------------------------------------
tcp::socket& Session::get_socket()
{
    return m_Socket;
}
//-----------------------------------------------------------------------------
void Session::start_async_read()
{
    m_Socket.async_read_some(
        boost::asio::buffer(m_Data, BUFFER_SIZE),
        boost::bind(&Session::handle_read, this, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}
//-----------------------------------------------------------------------------
void Session::handle_read(std::shared_ptr<Session>& s, const boost::system::error_code& e, size_t bytes)
{
    std::string client_address = utils::get_socket_address(s);

    if (e)
    {
        //���������, � ����� ������ ����������
        if (e == boost::asio::error::connection_reset ||
            e == boost::asio::error::eof)
        {
            std::cout << "Disconnected " << client_address << std::endl;
        }
        else //� ��� ��� ��� ������
        {
            std::cout << "Can't read data from client " << client_address << ": " << e.message() << std::endl;
        }
        return;
    }

    //��������� ��������� ������� �� ������� � ������
    auto commands = utils::split_string(std::string(m_Data.begin(), m_Data.begin() + bytes), '\n');
    for (const auto& command : commands)
    {
        SessionContext ctx;

        //��������� ��������� �������, ��������� ����� � ����������� �� ���������� � �������� �������
        bool res = execute_command(ctx, command);
        
        std::string answer;
        if (res)
        {
            answer = "OK";

            if (!ctx.Answer.empty())
            {
                answer += "\n" + ctx.Answer;
            }
        }
        else
        {
            answer = ctx.ErrorMessage;
        }
        answer += "\n";

        s->get_socket().write_some(boost::asio::buffer(answer, answer.size()));
    }

    //������ ����� � ��������� ������� ������������ ������
    std::fill(m_Data.begin(), m_Data.end(), '\0');
    start_async_read();
}
//-----------------------------------------------------------------------------
bool Session::execute_command(SessionContext& ctx, const std::string& cmd)
{
    //������ �������
    auto v = utils::split_string(cmd, ' ');
    size_t v_size = v.size();
    if (v_size == 0)
    {
        ctx.ErrorMessage = "Invalid format!";
        return false;
    }

    std::string command_type = v.front();
    utils::string_to_lower(command_type);

    if (command_type == "select" && v_size == 2)
    {
        return execute_select(ctx, v[1]);
    }
    else if (command_type == "selectall")
    {
        return execute_selectall(ctx);
    }
    else if (command_type == "insert" && v_size == 4)
    {
        return execute_insert(ctx, v);
    }
    else if (command_type == "delete" && v_size == 3)
    {
        return execute_delete(ctx, v[1], v[2]);
    }
    else if (command_type == "truncate" && v_size == 2)
    {
        return execute_truncate(ctx, v[1]);
    }
    else if (command_type == "intersection" && v_size == 1)
    {
        return execute_intersection(ctx);
    }
    else
    {
        ctx.ErrorMessage = "Invalid command: " + command_type;
    }

    return false;
}
//-----------------------------------------------------------------------------
bool Session::execute_select(SessionContext& ctx, const std::string& table_name)
{
    //��������, ���� �� ����� �������
    Table* tbl = get_table(table_name, &ctx);
    if (!tbl)
    {
        return false;
    }

    ctx.Answer += table_name + ":\n";

    //���� ������� ������, ��� � �������
    if (tbl->empty())
    {
        ctx.Answer += "Table \"" + table_name + "\" is empty";
    }
    else //������� �� ������ - �������
    {
        for (const auto& record : (*tbl))
        {
            ctx.Answer += std::to_string(record.ID) + "\t" + record.Name + "\n";
        }
    }

    return true;
}
//-----------------------------------------------------------------------------
bool Session::execute_selectall(SessionContext& ctx)
{
    //����������� �� ��������
    for (const auto& table : m_Database)
    {
        if (!execute_select(ctx, table.first))
        {
            return false;
        }
        ctx.Answer += "\n";
    }
    return true;
}
//-----------------------------------------------------------------------------
bool Session::execute_insert(SessionContext& ctx, const std::vector<std::string>& insert_vec)
{
    std::string table_name = insert_vec[1];

    uint64_t id = 0;
    if (auto a = utils::string_to_uint64(insert_vec[2]); a)
    {
        id = a.value();
    }
    else
    {
        ctx.ErrorMessage = "Invalid ID!";
        return false;
    }

    std::string name = insert_vec[3];

    //��������, ���� �� ����� �������
    auto it = m_Database.find(table_name);
    if (it != m_Database.end())
    {
        //������� ���� - ��������, ��� �� ��� ����� ������

        bool found = false;
        for (const Record& record : it->second)
        {
            found = record.ID == id;
            if (found)
            {
                ctx.ErrorMessage = "ERR duplicate " + insert_vec[2];
                return false;
            }
        }

        it->second.emplace_back(Record{ id, name });
    }
    else //������� ��� - ��������� ������� � ������ � ��
    {
        m_Database[table_name].emplace_back(Record{ id, name });
    }

    return true;
}
//-----------------------------------------------------------------------------
bool Session::execute_delete(SessionContext& ctx, const std::string& table_name, const std::string& id_str)
{
    //��������, ���� �� ����� �������
    Table* tbl = get_table(table_name, &ctx);
    if (!tbl)
    {
        return false;
    }

    auto id = string_to_uint64(ctx, id_str);
    if (!id)
    {
        return false;
    }

    bool found = false;

    //������ ������ � ��������������� id
    for (size_t i = 0, c = tbl->size(); i < c; ++i)
    {
        Record& record = (*tbl)[i];
        found = record.ID == id;

        //���� ������ ����� - ������� �
        if (found)
        {
            auto it_beg = tbl->begin();
            std::advance(it_beg, i);
            tbl->erase(it_beg);
            break;
        }
    }

    //������� �������, ��� ������ � ����� id ���
    if (!found)
    {
        ctx.ErrorMessage = "Record with id " + id_str + " not found";
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
bool Session::execute_truncate(SessionContext& ctx, const std::string& table_name)
{
    Table* tbl = get_table(table_name, &ctx);
    if (!tbl)
    {
        return false;
    }

    m_Database.erase(table_name);
    return true;
}
//-----------------------------------------------------------------------------
bool Session::execute_intersection(SessionContext& ctx)
{
    size_t table_count = m_Database.size();
    if (table_count == 1)
    {
        ctx.ErrorMessage = "ERR for this operator you need to have more one table";
        return false;
    }
    --table_count;

    //�������� ������ �� ������ �������. �� �� � ����� �������������
    const Table& first_table = m_Database.begin()->second;

    //�������, � ������� ������ �������������, � ������ ������, � ������� ���� ������������� ����� ������
    std::unordered_map<uint64_t, std::vector<std::string>> m_temp;

    //�������� �������� ��������� ������: ����������� �� ���� ��������, ����� ������
    for (auto it = std::next(m_Database.begin()); it != m_Database.end(); ++it)
    {
        //�� ��� ����������� �� ������ ������� � ���� ��������� ��������������� � ������ ��������
        for (const auto& record : first_table)
        {
            //���� ����� - ��������� �����-�� ������������� � �������, � ������� �� ��� ������
            if (exists_id(it->second, record.ID))
            {
                m_temp[record.ID].emplace_back(it->first);
            }
        }

    }

    //����������� �� ������� ��������� ����������
    for (auto it = m_temp.begin(); it != m_temp.end(); ++it)
    {
        //���������� ��������������, ������� �� ���������� �� ���� ��������
        if (it->second.size() != table_count)
        {
            continue;
        }

        ctx.Answer += std::to_string(it->first) + "," + get_name(m_Database.begin()->first, it->first) + ",";

        for (const std::string& table_name : it->second)
        {
            ctx.Answer += get_name(table_name, it->first) + ",";
        }

        utils::string_rm_right(ctx.Answer, 1);
        ctx.Answer += "\n";
    }

    return true;
}
//-----------------------------------------------------------------------------
std::optional<uint64_t> Session::string_to_uint64(SessionContext& ctx, const std::string& s)
{
    uint64_t id = 0;
    if (auto a = utils::string_to_uint64(s); a)
    {
        id = a.value();
    }
    else
    {
        ctx.ErrorMessage = "Invalid ID";
        return std::nullopt;
    }

    return id;
}
//-----------------------------------------------------------------------------
Session::Table* Session::get_table(const std::string& table_name, SessionContext* ctx)
{
    auto it = m_Database.find(table_name);
    if (it == m_Database.end())
    {
        if (ctx)
        {
            ctx->ErrorMessage = "Table \"" + table_name + "\" not found";
        }
        return nullptr;
    }
    return &it->second;
}
//-----------------------------------------------------------------------------
bool Session::exists_id(const Table& table, uint64_t id)
{
    for (const auto& record : table)
    {
        if (record.ID == id)
        {
            return true;
        }
    }
    return false;
}
//-----------------------------------------------------------------------------
std::string Session::get_name(const std::string& table_name, uint64_t id)
{
    Table* table = get_table(table_name);
    for (const auto& record : (*table))
    {
        if (record.ID == id)
        {
            return record.Name;
        }
    }
    return std::string();
}
//-----------------------------------------------------------------------------
