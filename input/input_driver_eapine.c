#include "input_driver_eapine.h"


const char* eapine_message_to_string(uint16_t e_type)
{
   switch (e_type) {
   case SearchHost: return "SearchHost";
   case JoypadStates: return "JoypadStates";
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
      case SearchHost:
      {
         CS_SearchHost_handle(input_st->remote->net_fd[user], (struct sockaddr*)&addr, addr_size);
         break;
      }
      case JoypadStates:
      {
         CS_JoypadStates_handle(&buffer[offset], &input_st->remote_st_ptr, user);
         break;
      }
      default:
         break;
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

void CS_SearchHost_handle(SOCKET socket, struct sockaddr* addr, socklen_t addr_size)
{
   struct sockaddr_in* addr_in = addr;
   RARCH_LOG("[Eapine] CS_SearchHost_handle %s %d.\n", inet_ntoa(addr_in->sin_addr), addr_in->sin_port);

   struct eapine_joypad_slot_states msg;
   msg.key = SearchHost;
   
   msg.platform = 1;
   msg.length = 4;
   msg.state = 2;
   
   sendto(socket, (char*)&msg, sizeof(msg), 0, addr, addr_size);
}


void CS_JoypadStates_handle(eapine_joypad_states_t* msg, input_remote_state_t* input_state, unsigned user)
{
   input_state->buttons[user] = msg->joypad_states;
   eapine_input_timer_ptr.joypads[user] = cpu_features_get_time_usec();

   RARCH_LOG("[Eapine] JoypadStates_handle %d.\n", msg->joypad_states);
}
