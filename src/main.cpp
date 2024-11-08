
#include "main.h"
#include "ui.h"

//#define OVRSERIAL 1

using namespace std;

extern CustomSerialPort sp;
extern CustomCANPort cp;

int main()  {
    char command, ser_cmd;
    void *psport = NULL;

#if defined(OVRSERIAL)
    psport = &sp;
#else
    psport = &cp;
#endif
    cout << "\r\n I am here\r\n";

    while(1)    {
        scanf(" %c", &command);
        if(command == 'q') {
            break;
        }
        ser_cmd = follow_command(&command, &psport);
    }

    return 0;
}