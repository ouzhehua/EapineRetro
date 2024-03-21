#include "input_driver_eapine.h"


const char* eapine_message_to_string(uint16_t e_type)
{
   switch (e_type) {
   case MSG_JoypadStates: return "MSG_JoypadStates";
   default: return "UNKNOWN";
   }
}

void eapine_message_parse(input_driver_state_t* input_st, unsigned user)
{
   fd_set fds;
   ssize_t ret;
   char buffer[1024];

   struct sockaddr_storage addr = { 0 };
   socklen_t addr_size = sizeof(addr);

   if (input_st->remote->net_fd[user] < 0)
      return;

   FD_ZERO(&fds);
   FD_SET(input_st->remote->net_fd[user], &fds);

   ret = recvfrom(input_st->remote->net_fd[user], buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_size);

   if (ret != -1)
   {
      if (ret < 2)
      {
         RARCH_ERR("[Eapine] eapine_message_parse length:%d too short.\n", ret);
         return;
      }

      uint16_t* key = &buffer[0];
      uint16_t offset = sizeof(uint16_t);

      RARCH_LOG("[Eapine] MSG:%d %s\n", *key, eapine_message_to_string(*key));

      switch (*key)
      {
      case MSG_JoypadStates:
      {
         MSG_JoypadStates_handle(&buffer[offset], &input_st->remote_st_ptr, user);
         break;
      }
      }
   }
}

void eapine_check_input_valid(input_remote_state_t* input_state, unsigned user)
{
   //Joypad
   if (input_state->buttons[user] != 0)
   {
      retro_time_t delta = cpu_features_get_time_usec() - eapine_input_timer_ptr.joypads[user];
      
      if (delta > 3000000)//超过3秒没有输入,认为断线
      {
         input_state->buttons[user] = 0;
      }
   }
}

void MSG_JoypadStates_handle(eapine_joypad_states_t* msg, input_remote_state_t* input_state, unsigned user)
{
   input_state->buttons[user] = msg->joypad_states;
   eapine_input_timer_ptr.joypads[user] = cpu_features_get_time_usec();

   RARCH_LOG("[Eapine] JoypadStates_handle %d.\n", msg->joypad_states);
}
