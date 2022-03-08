#include "tuya_ble_product_test_demo.h"
#include "tuya_ble_api.h"
#include "tuya_ble_utils.h"




/*********************************************************************
 * LOCAL CONSTANT
 */

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
static uint8_t tx_buffer[128] = {0x66, 0xAA, 0x00, 0xF0};

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: 
*/
void tuya_ble_custom_app_production_test_process(uint8_t channel, uint8_t* p_in_data, uint16_t in_len)
{
    ty_product_test_cmd_t* cmd = (void*)p_in_data;
    tuya_ble_inverted_array((void*)&cmd->len, sizeof(uint16_t));
    tuya_ble_inverted_array((void*)&cmd->sub_id, sizeof(uint16_t));
    
    if((cmd->type != 3) || (cmd->len < 3)){
        return;
    }
    
    uint16_t data_len = cmd->len - 3;
    
    switch(cmd->sub_id)
    {
		case PRODUCT_TEST_CMD_ENTER: {
            uint8_t tmp_buf[] = "{\"ret\":true}";
            tuya_ble_product_test_rsp(channel, cmd->sub_id, tmp_buf, strlen((void*)tmp_buf));
        } break;
        
		case PRODUCT_TEST_CMD_EXIT: {
            uint8_t tmp_buf[] = "{\"ret\":true}";
            tuya_ble_product_test_rsp(channel, cmd->sub_id, tmp_buf, strlen((void*)tmp_buf));
        } break;
        
        default: {
        } break;
    }
}

/*********************************************************
FN: 
*/
uint32_t tuya_ble_product_test_rsp(uint8_t channel, uint16_t cmdId, uint8_t* buf, uint16_t size)
{
    uint32_t len = 4;
    
    tx_buffer[len] = (size+3)>>8;
    len++;
    tx_buffer[len] = (size+3)&0xFF;
    len++;
    
    tx_buffer[len] = 0x03;
    len++;
    
    tx_buffer[len] = cmdId>>8;
    len++;
    tx_buffer[len] = cmdId&0xFF;
    len++;
    
    if(size > 0) {
        memcpy(&tx_buffer[len], buf, size);
        len += size;
    }
    
    tx_buffer[len] = tuya_ble_check_sum(tx_buffer, len);
    len += 1;
    
    tuya_ble_production_test_asynchronous_response(channel, tx_buffer, len);
    
    return 0;
}





















