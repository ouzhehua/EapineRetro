#ifndef __EAPINE_DRIVER__H
#define __EAPINE_DRIVER__H

/// <summary>
/// Message 枚举
/// </summary>
enum eapine_message
{
   MSG_JoypadStates = 1,//同步虚拟手柄Joypad的状态
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
/// 处理 MSG_JoypadStates
/// </summary>
void MSG_JoypadStates_handle(eapine_joypad_states_t* msg, input_remote_state_t* input_state, unsigned user);

#endif /* __EAPINE_DRIVER__H */
