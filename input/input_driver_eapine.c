#include "input_driver_eapine.h"
#include "../menu/menu_cbs.h"
#include "../tasks/task_file_transfer.h"

const char* eapine_message_to_string(uint16_t e_type)
{
   switch (e_type) {
   case SearchHost: return "SearchHost";
   case JoypadStates: return "JoypadStates";
   case DownloadGameRom: return "DownloadGameRom";
   case UploadGameData: return "UploadGameData";
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
      case DownloadGameRom:
      {
         CS_DownloadGameRom_handle(&buffer[offset]);
         break;
      }
      case UploadGameData:
      {
         CS_UploadGameData_handle(&buffer[offset]);
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

void CS_SearchHost_handle(int socket, struct sockaddr* addr, socklen_t addr_size)
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

void CS_DownloadGameRom_handle(char* buffer)
{
   char* url = buffer;

   uint16_t offset = strlen(url) + 2;
   buffer += offset;

   char* path = buffer;//全路径就放在具体路径，只有文件名就放在Download文件夹下
   
   RARCH_LOG("[Eapine] CS_DownloadGameRom_handle %s %s.\n", url, path);

   file_transfer_t* transf = NULL;
   transf           = (file_transfer_t*)malloc(sizeof(*transf));
   transf->enum_idx = MENU_ENUM_LABEL_CB_CORE_CONTENT_DOWNLOAD;
   strlcpy(transf->path, path, sizeof(transf->path));

   task_push_http_transfer_file(url, false,
      msg_hash_to_str(MENU_ENUM_LABEL_CB_CORE_CONTENT_DOWNLOAD), cb_generic_download, transf);
}

void cb_game_rom_download(retro_task_t* task, void* task_data, void* user_data, const char* err)
{
   RARCH_LOG("[Eapine] cb_game_rom_download.\n");
}

void CS_UploadGameData_handle(char* buffer)
{
   // 游戏文件数据
   uint16_t consoleType = buffer;

   RARCH_LOG("[Eapine] CS_UploadGameData_handle %d.\n", consoleType);

   //buffer += sizeof(uint16_t);
   //uint16_t platform = buffer;

   //public ConsoleEnum consoleType;//游戏机类型
      //public string fileName;//游戏文件名
      //public string extension;//游戏文件扩展名
      //public byte[] fileData;//游戏数据
}
