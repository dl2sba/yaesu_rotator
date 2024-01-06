#ifndef _YAESU_GS232A
#define _YAESU_GS232A

#define GS232A_CMD_SE  0xF0  //  End of sub negotiation parameters
#define GS232A_CMD_NOP 0xF1  //  No Operation
#define GS232A_CMD_AYT 0xF6  //  Are You There
#define GS232A_CMD_SB  0xFA  //  Start of sub negotiation parameters
#define GS232A_CMD_IAC 0xFF  //  Command identifier

#define G323A_ROTATE_STEP 5
#define G323A_MAX_ANGLE   360

class YAESU_GS232A_Class {
  public:
    void init(void);
    void processCommands(WiFiClient & client);

  public:
    uint32_t  numCmdIAC = 0;
    uint32_t  numCmdC = 0;
    uint32_t  numCmdW = 0;
    uint32_t  numCmdR = 0;
    uint32_t  numCmdL = 0;
    uint32_t  numCmdS = 0;
    uint32_t  numCmdA = 0;
    uint32_t  numCmdE = 0;
    uint32_t  numCmdM = 0;
    uint32_t  numCmdU = 0;
    uint32_t  numCmdD = 0;
    uint32_t  numCmdX = 0;
};

#endif
