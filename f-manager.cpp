#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<vector>
#include<thread>
#include <mutex>
#include <cstdio> 
#include <termios.h>
#include <unistd.h>


#define RESET   "\033[0m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"


using namespace std;


vector<string> clipboard_history;
mutex history_mutex; 
int selected_index = -1;
int count_pages = 0;
int count_logs ;
int count_all;
bool first_time = false;
bool stats_view = false;
auto start_time = std::chrono::steady_clock::now();
bool date_view = false;



void set_color(const string& color_code) 
{
    cout << color_code;
}



string print_uptime() 
{
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
    return std::to_string(elapsed.count());
}



void erease_clipboard_history()
{
    clipboard_history.clear();
} 



void something_went_wrong(string m)
{
        set_color("\033[31m");
        cout<<m<<endl;
        set_color("\033[0m");
}



std::string get_home_dir()
{
    const char* homeDir = getenv("HOME");
    if(homeDir == NULL)
    {
        something_went_wrong("something went wrong...");
        return 0;
    }
    return std::string(homeDir);
}



void animation()
{
    cout << "" << endl;
    cout << "" << endl;
    cout << "       /\\_/\\" << std::endl;
    cout << "      ( o.o )" << std::endl;
    cout << "       > ^ <  /" << std::endl;
    cout << "      /       |" << std::endl;
    cout << "     (  |  |  )" << std::endl;
    cout << "    /   |  |   \\" << std::endl;
    cout << "   (____|_____)";
    cout <<"";
    
}



char getch() 
{
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}



void save_to_file(string mode,string path,string content)
{
    std::string filePath = get_home_dir() + path;
    FILE* file = fopen(filePath.c_str(), mode.c_str());
    if(file == nullptr)
    {
        something_went_wrong("something went wrong...");
        return;
    }
    if(file)
    {
        if(content=="ch")
        {
        for(int i = 0; i < clipboard_history.size(); i++)
        {
            fwrite(clipboard_history[i].c_str(), sizeof(char), clipboard_history[i].length(), file);
            fwrite("\n", sizeof(char), 1, file);
        }
        }
        else
        {
            fwrite(content.c_str(), sizeof(char), content.length(), file);
            fwrite("\n", sizeof(char), 1, file);
        }
        fclose(file);
    }
}



vector<string> read_file(string path)
{
    vector<string> res;
    std::string filePath = get_home_dir() + path;
    FILE* file = fopen(filePath.c_str(), "r");
    if(file == nullptr)
    {
        count_logs++;
        first_time = true;
        return res;
    }
    if(file)
    {
        char buffer[1024];
        while(fgets(buffer, sizeof(buffer), file))
        {
            std::string line(buffer);
            res.push_back(buffer);
        }
        fclose(file);
    }
    return res;
}



void read_statistics()
{
    if(first_time)
    {
        save_to_file("w","/Desktop/Data/cs/projects/file-manager/clipboard_stats.txt",to_string(count_logs));
        save_to_file("a","/Desktop/Data/cs/projects/file-manager/clipboard_stats.txt",to_string(count_all));
    }
    vector<string> data = read_file("/Desktop/Data/cs/projects/file-manager/clipboard_stats.txt");
    count_logs = stoi(data[0]);
    count_all = stoi(data[1]);
}


void save_statistics()
{
    count_logs++;
    save_to_file("w","/Desktop/Data/cs/projects/file-manager/clipboard_stats.txt",to_string(count_logs));
    save_to_file("a","/Desktop/Data/cs/projects/file-manager/clipboard_stats.txt",to_string(count_all));
    }


string get_clipboard_data()
{
    string data;
    char buffer[1024];
    string command = "pbpaste";
    FILE *file = popen(command.c_str(), "r");
    if (file) 
    {
            while (fgets(buffer, sizeof(buffer), file) != NULL) 
            {
                data += buffer;
            }
        pclose(file); 
    }
    else
    {
        something_went_wrong("something went wrong....");
    }
    return data;
}


bool check_for_double_data(string s)
{
    for(int x = 0; x < clipboard_history.size(); x++)
    {
        if(clipboard_history[x] == s)
        {
            return true;
        }
   }
   return false;
}


void save_clipboard_Data()
{
    if (clipboard_history.empty() || clipboard_history.back() != get_clipboard_data() && !check_for_double_data(get_clipboard_data()))
    {
            clipboard_history.push_back(get_clipboard_data());
            count_all++;
            selected_index = clipboard_history.size()-1;
    }
}



void keep_last_two_lines()
{
    clipboard_history.erase(clipboard_history.begin(), clipboard_history.end() - 2);
    for(int i = 0; i < clipboard_history.size(); i++)
    {
        save_to_file("w","/Desktop/Data/cs/projects/file-manager/clipboard_history.txt","ch");
    }
}



bool check_for_empty_data(string s)
{ //useless i guess
    int count = 0;
    for(int i = 0;i<s.length();i++)
    {
        if(s[i] == ' ')
        {
            count++;
            if (count >= 4)
            {
                return true;
            }
        }
    }
    return false;
}

void prompt()
{
    set_color(RESET);
    cout<<""<<endl<<endl<<endl;
    cout << "press [i]ndex to copy, press e to edit fav"<< endl;
    cout<<"press q save and quit"<<endl;
    cout<<"command :"<<endl;
}



void show_clipboard_data(string mode)
{
    system("clear");
    if(mode=="with_stats")
    {
        cout << "[state]          [i]  [content]          Total: "<<count_all<<
        "  logs: "<<count_logs<<endl;
        cout << "__________________________________________________" << endl<< endl;
    }
    if(mode=="without_stats")
    {
        cout << "[state]          [i]  [content]     saved in /file-manager/clipboard_history.txt" << endl;
        cout << "__________________________________________________" << endl<< endl;
    }
    if(mode=="date")
    {
        cout << "[state]          [i]  [content]"<<"         uptime: "<<print_uptime()<<endl;
        cout << "__________________________________________________" << endl<< endl;
    }
    for (int i = 0; i < clipboard_history.size(); i++) 
    {
        string res = clipboard_history[i];
        string state = "";
        if (clipboard_history[i] != "" && clipboard_history[i] != "\n"&& 
            clipboard_history[i] != " " && clipboard_history[i] != "\t") //useless again
            {   
            if (res.size() > 30) 
            {
                res = res.substr(0, 30) + "..."; 
            }
            else
            {
                state = "used recently";
            }

            if(i==selected_index)
            {
                set_color(RED);
            }     
            else
            {
            set_color(YELLOW);
            }            
            if(i==0&&count_pages ==0||i==1&& count_pages==0)
            { 
                state = "used often/favs";
                cout <<state<<"  [" << i + 1 << "]  " << res;   
            }
            else
            {
            state = "used recently";
            cout <<state<<"    [" << i + 1 << "]  " << res<<endl;
            set_color(YELLOW);
            }
        } 
        if(clipboard_history.empty())
        { 
            something_went_wrong("  [1]  no data; clipboard is empty :(");
        }
    }
}
    


void check_length()
{
    if(clipboard_history.size() >= 10)
    {
        count_pages++;
        erease_clipboard_history();
    }
}



void copy_to_clipboard(int index)
{
    lock_guard<mutex> lock(history_mutex);
    FILE *file = popen("pbcopy", "w");
    if (file)
    {
        fwrite(clipboard_history[index].c_str(),sizeof(char),clipboard_history[index].length(),file);
        pclose(file);
        selected_index = index; 
    }
    else
    {
        something_went_wrong("something went wrong...");
    }
}



std::string trim_end(std::string s) 
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) 
    {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}



void open_file() 
{
    string command = get_home_dir() + "/Desktop/Data/cs/projects/file-manager/clipboard_history.txt";
    pid_t pid = fork();
    if (pid == 0) 
    { // Child process
        execlp("open", "open", command.c_str(), nullptr);
        exit(0); // Exit child process if execlp fails
    } else if (pid < 0) 
    {
        something_went_wrong("Failed to fork process");
    }
}



void check_view()
{
    if(stats_view)
    {
        show_clipboard_data("with_stats");
    }
    else if(date_view)
    {
        show_clipboard_data("date");
    }
    else
    {
        show_clipboard_data("without_stats");
    }
    prompt();
}


void copy_to_clipboard_interface() 
{
    int s_count = 0;
    while (true) 
    {
        char key = getch();  

        if (key == 'q') 
        {  
            save_statistics();
            keep_last_two_lines();
            exit(0);
        } 
        if (key == 'e') 
        {  
            open_file();
        }
        if (key == 's') 
        {  
            s_count++;
            switch (s_count)
            {
            case 1:
                stats_view = true;
                date_view = true;
                break;
            case 2:
                stats_view = false;
                date_view = true;
                break;  
            case 3:
                stats_view = false;
                date_view = false;
                s_count = 0;
                break;
            }            
        }
        else if (key >= '1' && key <= '9') 
        {  
            int index = key - '1';  
            if (index < clipboard_history.size()) 
            {
                copy_to_clipboard(index);
            }
        }
    }
}



void get_all_data()
{
    system("pbcopy < /dev/null"); 
    vector<string> data = read_file("/Desktop/Data/cs/projects/file-manager/clipboard_history.txt");
    clipboard_history = data;
    read_file("/Desktop/Data/cs/projects/file-manager/clipboard_stats.txt");
    read_statistics();
}


thread start(){
    thread user_input_thread(copy_to_clipboard_interface); 
    animation();
    this_thread::sleep_for(chrono::milliseconds(1500));
    get_all_data();
    return user_input_thread;
}



void loop(){
    thread user_input_thread = start();
    while(true)
    {
        save_clipboard_Data();
        check_view();
        check_length();
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    user_input_thread.join();
}


int main()
{
    loop();
    return 0;
}        