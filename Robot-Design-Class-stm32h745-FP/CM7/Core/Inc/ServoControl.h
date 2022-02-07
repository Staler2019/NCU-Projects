/*
@Copyright (c) 2021 MIAT LAB
@Description
    use for robot design class
*/
void robotInit(UART_HandleTypeDef *huart,
               uint32_t arrive_time)
{
    uint32_t servo_amount = 6;
    uint32_t servo_list[6] = {1, 2, 3, 4, 5, 6};
    uint32_t servo_pwm_list[6] = {1500, 1500, 1500,
                                  1500, 1500, 1500};
    robot_control_cmd(huart, servo_amount, arrive_time,
                      servo_list, servo_pwm_list);
}

int robot_control_cmd(UART_HandleTypeDef *huart,
                      uint32_t servo_amount,
                      uint32_t arrive_time,
                      uint32_t *servo_list,
                      uint32_t *servo_pwm_list)
{
    uint32_t length = servo_amount * 3 + 5;
    uint8_t *cmd_data =
        (uint8_t *)malloc(sizeof(uint8_t) * (length + 2));
    cmd_data[0] = 0x55;
    cmd_data[1] = 0x55;
    cmd_data[2] = length;
    cmd_data[3] = 0x03;
    cmd_data[4] = servo_amount;
    cmd_data[5] = (arrive_time << 8 & 0xffff) >> 8;
    cmd_data[6] = (arrive_time & 0xffff) >> 8;
    uint32_t i = 0, k = 7;
    while (k < length + 2) {
        cmd_data[k] = servo_list[i];
        k++;
        cmd_data[k] =
            (servo_pwm_list[i] << 8 & 0xffff) >> 8;
        k++;
        cmd_data[k] = servo_pwm_list[i] >> 8;
        k++;
        i++;
    }
    HAL_UART_Transmit(huart, cmd_data, length + 2,
                      HAL_MAX_DELAY);
    HAL_Delay(arrive_time);
    return 1;
}
int robotRotate(UART_HandleTypeDef *huart,
                uint32_t arrive_time, uint32_t servo_number,
                uint32_t servo_pwm)
{
    uint32_t servo_list[1] = {servo_number};
    uint32_t servo_pwm_list[1] = {servo_pwm};
    robot_control_cmd(huart, 1, arrive_time, servo_list,
                      servo_pwm_list);
    return 1;
}
int robot_putDown(UART_HandleTypeDef *huart,
                  uint32_t arrive_time)
{
    uint32_t servo_list[1] = {6};
    uint32_t servo_pwm_list[1] = {950};
    robot_control_cmd(huart, 1, arrive_time, servo_list,
                      servo_pwm_list);
    return 1;
}
int robot_pickUp(UART_HandleTypeDef *huart,
                 uint32_t arrive_time)
{
    uint32_t servo_list[1] = {6};
    uint32_t servo_pwm_list[1] = {1700};
    robot_control_cmd(huart, 1, arrive_time, servo_list,
                      servo_pwm_list);
    return 1;
}
