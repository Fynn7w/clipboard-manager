#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<vector>
#include<thread>
#include <mutex>
#include <cstdio> 


#define RESET   "\033[0m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"



using namespace std;


vector<string> clipboard_history;
mutex history_mutex; 


void set_color(const string& color_code) {
    cout << color_code;
}


void erease_clipboard_history(){
    clipboard_history.clear();
} 


std::string get_current_time() {
    std::time_t now = std::time(nullptr);
    return std::ctime(&now);
}


void something_went_wrong(string m){
        set_color("\033[31m");
        cout<<m<<endl;
        set_color("\033[0m");
}

std::string get_home_dir(){
    const char* homeDir = getenv("HOME");
    if(homeDir == NULL){
        something_went_wrong("something went wrong...");
        return 0;
    }
    return std::string(homeDir);
}

void save_to_file(string mode){
    std::string filePath = get_home_dir() + "/Desktop/Data/cs/projects/file-manager/clipboard_history.txt";
    FILE* file = fopen(filePath.c_str(), mode.c_str());
    if(file == nullptr){
        something_went_wrong("something went wrong...");
        return;
    }
    if(file){
        for(int i = 0; i < clipboard_history.size(); i++){
            fwrite(clipboard_history[i].c_str(), sizeof(char), clipboard_history[i].length(), file);
            fwrite("\n", sizeof(char), 1, file);
        }
        fclose(file);
    }
}



void read_file(){
    std::string filePath = get_home_dir() + "/Desktop/Data/cs/projects/file-manager/clipboard_history.txt";
    FILE* file = fopen(filePath.c_str(), "r");
    if(file == nullptr){
        something_went_wrong("something went wrong...");
        return;
    }
    if(file){
        char buffer[1024];
        while(fgets(buffer, sizeof(buffer), file)){
            std::string line(buffer);
            clipboard_history.push_back(buffer);
        }
        fclose(file);
    }
}



string get_clipboard_data(){
    string data;
    char buffer[1024];
    string command = "pbpaste";
    FILE *file = popen(command.c_str(), "r");
    if (file) {
            while (fgets(buffer, sizeof(buffer), file) != NULL) {
                data += buffer;
            }
        pclose(file); 
    }
    else{
        something_went_wrong("something went wrong....");
    }
    return data;
}

bool check_for_double_data(string s){
    for(int x = 0; x < clipboard_history.size(); x++){
        if(clipboard_history[x] == s){
            return true;
        }
   }
   return false;
}


void save_clipboard_Data(){
    if (clipboard_history.empty() || clipboard_history.back() != get_clipboard_data() && !check_for_double_data(get_clipboard_data())){
            clipboard_history.push_back(get_clipboard_data());
    }
}


void keep_last_two_lines(){
    clipboard_history.erase(clipboard_history.begin(), clipboard_history.end() - 2);
    save_to_file("w");
}


bool check_for_empty_data(string s){ //useless i guess
    int count = 0;
    for(int i = 0;i<s.length();i++){
        if(s[i] == ' '){
            count++;
            if (count >= 4){
                return true;
            }
        }
    }
    return false;
}


void show_clipboard_data(){
    system("clear");
    cout << "[state]         [i]  [content]" << endl;
    cout << "__________________________________" << endl<< endl;
    for (int i = 0; i < clipboard_history.size(); i++) {
        string res = clipboard_history[i];
        if (clipboard_history[i] != "" && clipboard_history[i] != "\n"&& 
            clipboard_history[i] != " " && clipboard_history[i] != "\t"){ //not really needed
            if (res.size() > 30) {
                res = res.substr(0, 30) + "..."; 
            }
            if(i==0||i==1){ //on screen 1/2
                set_color("\033[33m");
                cout << "used last time  [" << i + 1 << "]  " << res;
                set_color("\033[0m");
            }
            else{
            set_color("\033[33m");
            cout<<"used recently   [" << i + 1 << "]  " << res<<endl;
            set_color("\033[0m");
            }
        } 
        if(clipboard_history.empty()){ 
            something_went_wrong("  [1]  no data; clipboard is empty :(");
        }
    }
}
    

void copy_to_clipboard(int index){
    lock_guard<mutex> lock(history_mutex);
    FILE *file = popen("pbcopy", "w");
    if (file){
        fwrite(clipboard_history[index].c_str(),sizeof(char),clipboard_history[index].length(),file);
        pclose(file);
        }
    else{
        something_went_wrong("something went wrong...");
    }
}

std::string trim_end(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}


void copy_to_clipboard_interface(){
    std::string s = "";
    int index;
    string path = get_home_dir() + "/Desktop/Data/cs/projects/file-manager/clipboard_history.txt";
    while (true) {
    cin>>index;
    switch(index){
        case 0:
            save_to_file("a");
            keep_last_two_lines();
            exit(0);
            break;
        default:
            if(index > 0 && index < clipboard_history.size()){
                copy_to_clipboard(index-1);
                }
            if(index > clipboard_history.size()||index < 0){
                something_went_wrong("invalid index");
                }
            break;
        }
    }
}


void prompt(){
    cout<<""<<endl<<endl<<endl;
    cout << "press [i]ndex to copy, press 0 quit"<< endl<< endl;
    cout<<"command :"<<endl;
}

void favourites(){
    
}

int main(){
    system("pbcopy < /dev/null"); 
    thread user_input_thread(copy_to_clipboard_interface);
    read_file();
    while(true){
        save_clipboard_Data();
        show_clipboard_data();
        prompt();
        this_thread::sleep_for(chrono::seconds(1));
    }
    user_input_thread.join();
    return 0;
}        