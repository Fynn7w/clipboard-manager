#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<vector>
#include<thread>
#include <mutex>



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


void something_went_wrong(string m){
        set_color("\033[31m");
        cout<<m<<endl;
        set_color("\033[0m");
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



void save_clipboard_Data(){
    if (clipboard_history.empty() || clipboard_history.back() != get_clipboard_data()){
            clipboard_history.push_back(get_clipboard_data());
    }
}



void show_clipboard_data(){
    system("clear");
    cout << "press index to copy" << endl << endl << endl;
    for (int i = 0; i < clipboard_history.size(); i++) {
        string res = clipboard_history[i];
        if (clipboard_history[i] != "") {
            if (res.size() > 30) {
                res = res.substr(0, 30) + "..."; 
            }
            set_color("\033[33m");
            cout << "  [" << i + 1 << "]  " << res << endl;
            set_color("\033[0m");
        } else {
            something_went_wrong("  [1]  no data; clipboard is empty :()");
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
 


void copy_to_clipboard_interface(){
    while (true) {
    int index;
    cin>>index;
    if(index >= 0 && index < clipboard_history.size()){
        copy_to_clipboard(index-1);
        }
    else{
        something_went_wrong("invalid index");
        }
    }
}


void prompt(){
    cout<<""<<endl<<endl<<endl;
    cout<<"command :"<<endl;
}


int main(){
    thread user_input_thread(copy_to_clipboard_interface);
    while(true){
        save_clipboard_Data();
        show_clipboard_data();
        prompt();
        this_thread::sleep_for(chrono::seconds(2));
    }
    user_input_thread.join();
    return 0;
}        