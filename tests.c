#include "client.h"

int main(int argc, char **argv, char **envp){

    message_t msg_1;
    fillMessageHeader(&msg_1, "300", 11111, 22222, "Miles", "Jessica", "This is my message!");
    printMessageHead(&msg_1, 1);
    
    message_t msg_2;
    

}
