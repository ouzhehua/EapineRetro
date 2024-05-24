#ifndef __EAPINE_DRIVER__H
#define __EAPINE_DRIVER__H

/// <summary>
/// Message 枚举
/// </summary>
enum eapine_message
{
   SearchHost = 1,         //搜索主机 CS SC
   JoypadStates = 2,       //同步虚拟手柄Joypad的状态 CS
   DownloadGameRom = 3,    //下载游戏Rom
   UploadGameData = 4,     //上传游戏数据 CS
};

/// <summary>
/// 插槽绑定状态
/// </summary>
struct eapine_joypad_slot_states {
   uint16_t key;     //msg key

   uint16_t platform; //平台
   uint16_t length;  //插槽数量
   uint16_t state;   //插槽状态
};

/// <summary>
/// 同步虚拟手柄Joypad的状态
/// </summary>
typedef struct eapine_joypad_states
{
   uint16_t joypad_states;
} eapine_joypad_states_t;

/// <summary>
/// 虚拟手柄输入计时器
/// </summary>
typedef struct eapine_input_timer
{
   retro_time_t joypads[MAX_USERS];
   retro_time_t analog[MAX_USERS];

} eapine_input_timer_t;

/// <summary>
/// 虚拟手柄输入计时器
/// </summary>
eapine_input_timer_t eapine_input_timer_ptr;

/// <summary>
/// 获取 Message 枚举 对应字符串
/// </summary>
const char* eapine_message_to_string(uint16_t e_type);

/// <summary>
/// 解析收到的Message数据
/// </summary>
void eapine_message_parse(input_driver_state_t* input_st, unsigned user);

/// <summary>
/// 检查当前虚拟手柄输入是否在时效内, 若超时则重置输入状态
/// </summary>
void eapine_check_input_valid(input_remote_state_t* input_state, unsigned user);

/// <summary>
/// 处理 CS_SearchHost
/// </summary>
void CS_SearchHost_handle(int socket, struct sockaddr* addr, socklen_t addr_size);

/// <summary>
/// 处理 CS_JoypadStates
/// </summary>
void CS_JoypadStates_handle(eapine_joypad_states_t* msg, input_remote_state_t* input_state, unsigned user);

/// <summary>
/// 处理 CS_DownloadGameRom
/// </summary>
void CS_DownloadGameRom_handle(char* buffer);
void cb_game_rom_download(retro_task_t* task, void* task_data, void* user_data, const char* err);

/// <summary>
/// 处理 CS_UploadGameData
/// </summary>
void CS_UploadGameData_handle(char* buffer);

#endif /* __EAPINE_DRIVER__H */
