#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<vector>
#include<thread>

using namespace std;

vector<string> clipboard_history;


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


void save_clipboard_Data(){
    if (clipboard_history.empty() || clipboard_history.back() != get_clipboard_data()){ {
        clipboard_history.push_back(get_clipboard_data());
    }
}
}

void show_clipboard_data(){
    system("clear");
    for(int i = 0;i<clipboard_history.size();i++){
        cout<<"["<<i<<"]"<<clipboard_history[i]<<endl;
    }
}


int main(){
    while(true){
        save_clipboard_Data();
        show_clipboard_data();
        this_thread::sleep_for(chrono::seconds(2));
    }
    return 0;
}