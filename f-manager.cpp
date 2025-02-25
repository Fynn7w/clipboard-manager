#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<vector>
#include<thread>
#include <mutex>


using namespace std;

vector<string> clipboard_history;
mutex history_mutex; 

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
        cout<<"something went wrong..";
    }
    return data;
}


bool check_for_empty_data(string s){
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
    cout<<"type the index of the data you want to copy to clipboard: "<<endl<<endl<<endl;
    for(int i = 0;i<clipboard_history.size();i++){
        if(clipboard_history[i] != ""){
            cout<<"  ["<<i+1<<"]  "<<clipboard_history[i]<<endl;
        }
        else{
            cout<<"  ["<<i+1<<"] "<<"No data --> clipboard is empty :("<<endl;
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
        cout<<"something went wrong.."<<endl;
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
        cout<<"Invalid index"<<endl;
    }
}
}


int main(){
        thread user_input_thread(copy_to_clipboard_interface);
    while(true){
        save_clipboard_Data();
        show_clipboard_data();
        this_thread::sleep_for(chrono::seconds(2));
    }
    user_input_thread.join();
    return 0;
}        