#include "allhead.h"


//平均值滤波
void ADC_DMA_Test(void)
{
    ADC_2 = 0;
    for(uint8_t i = 0; i < 30; i++)
    {
        ADC_2 += ADC2_Value[i];
    }
    ADC_1 = 0;
    for(uint8_t i = 0; i < 30; i++)
    {
        ADC_1 += ADC1_Value[i];
    }
}

//ADC中位值滤波【ADC转换10个值】用到冒泡排序
float Adc_Proc(uint32_t _buf[])
{
    uint8_t i, j,count;
    float Adc_Val = 0;
    float temp;
    for(i = 0; i < 10 - 1; i++)	//-1是因为不用跟自己比
    {
		count = 0;
        for(j = 0; j < 10 - 1 - i; j++)	//size-1-i是因为每一趟就会少一个数比较
        {
            if (_buf[j] < _buf[j + 1])	//降序排序
            {
                temp = _buf[j];
                _buf[j] = _buf[j + 1];
                _buf[j + 1] = temp;
				count = 1;
            }
        }
		if(0 == count)	//如果某一趟没有交换位置，则说明已经排好序，直接退出循环
		{
			break;
		}
    }
	//去掉最小和最大值然后取平均值
    for(i = 1; i < 10 - 1; i++)
	{
        Adc_Val += _buf[i];		
	}

    return (Adc_Val / 8) / 4096 * 3.3f;
}


//获取ADC的值函数
double getADC(ADC_HandleTypeDef *hadc)
{
    uint32_t adc;
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, 500); //等待采集结束，超时是500ms
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(hadc), HAL_ADC_STATE_REG_EOC))//读取ADC完成标志位
    {
        adc = HAL_ADC_GetValue(hadc);
        return adc * 3.3 / 4096;
    }

    return 0;	//错误返回0
}

//设置DAC值
void set_DAC_value(DAC_HandleTypeDef *hdac,uint32_t Channel,float Vol)
{
	uint16_t temp;
	temp = Vol*4096/3.3f;
	HAL_DAC_SetValue(hdac,Channel,DAC_ALIGN_12B_R,temp);	//设置DAC输出值
}
