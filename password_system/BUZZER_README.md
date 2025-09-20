# STM32F411CEU6 Buzzer Control Example

## Mô tả dự án
Dự án này minh họa cách điều khiển buzzer (passive buzzer) sử dụng STM32F411CEU6 với chân PA8 (Timer1 Channel 1) để tạo ra các âm thanh và giai điệu khác nhau.

## Cấu hình Hardware

### Kết nối mạch
```
STM32F411CEU6    NPN Transistor (2N2222/2N3904)    Passive Buzzer
PA8 (TIM1_CH1) -----> Base (B)
                     Collector (C) -----> Positive (+) của Buzzer
                     Emitter (E) -------> GND
                     
VCC (3.3V/5V) -----> Positive (+) của Buzzer (qua Collector)
GND ----------> Negative (-) của Buzzer và Emitter
```

### Danh sách linh kiện
- STM32F411CEU6 (STM32F411 BlackPill board)
- Passive Buzzer (5V buzzer khuyến nghị)
- NPN Transistor (2N2222, 2N3904 hoặc tương tự)
- Resistor 1kΩ (nối giữa PA8 và Base của transistor)
- Breadboard và jumper wires

## Cấu hình phần mềm

### Timer Configuration
- **Timer:** TIM1
- **Channel:** CH1 (PA8)
- **Prescaler:** 95 (để có 1µs per tick với 96MHz system clock)
- **PWM Mode:** PWM Generation CH1
- **Clock Source:** Internal Clock

### System Clock
- **HSE:** 25MHz external crystal
- **System Clock:** 96MHz
- **Timer Clock:** 96MHz / (95+1) = 1MHz = 1µs per tick

## Các hàm chính

### Hàm điều khiển cơ bản
```c
void Buzzer_Tone(uint32_t frequency, uint32_t duration);  // Tạo tone với tần số và thời gian xác định
void Buzzer_NoTone(void);                                 // Tắt buzzer
void Buzzer_Start(void);                                  // Bật PWM
void Buzzer_Stop(void);                                   // Tắt PWM
```

### Hàm nâng cao
```c
void Buzzer_PlayMelody(uint32_t *melody, uint32_t *durations, uint32_t length);  // Chơi giai điệu
void Buzzer_Demo(void);                                                          // Demo tất cả tính năng
```

### Hàm tiện ích
```c
void Buzzer_PlaySimpleBeep(void);    // Tiếng beep đơn giản
void Buzzer_PlayErrorBeep(void);     // Tiếng beep lỗi (3 tiếng ngắn)
void Buzzer_PlaySuccessBeep(void);   // Tiếng beep thành công (nốt tăng dần)
```

## Tần số các nốt nhạc

### Octave 4 (Thấp)
```c
#define NOTE_C4  262   // Do
#define NOTE_D4  294   // Re  
#define NOTE_E4  330   // Mi
#define NOTE_F4  349   // Fa
#define NOTE_G4  392   // Sol
#define NOTE_A4  440   // La
#define NOTE_B4  493   // Si
```

### Octave 5 (Cao)
```c
#define NOTE_C5  523   // Do cao
#define NOTE_D5  587   // Re cao
#define NOTE_E5  659   // Mi cao
#define NOTE_F5  698   // Fa cao
#define NOTE_G5  784   // Sol cao
#define NOTE_A5  880   // La cao
#define NOTE_B5  988   // Si cao
```

## Ví dụ sử dụng

### 1. Tạo một âm thanh đơn giản
```c
// Phát nốt La (440Hz) trong 1 giây
Buzzer_Tone(NOTE_A4, 1000);
Buzzer_NoTone();  // Tắt âm thanh
```

### 2. Tạo tiếng báo động
```c
for (int i = 0; i < 10; i++) {
    Buzzer_Tone(800, 200);  // Tần số cao
    Buzzer_Tone(400, 200);  // Tần số thấp
}
Buzzer_NoTone();
```

### 3. Chơi giai điệu tùy chỉnh
```c
uint32_t my_melody[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
uint32_t my_durations[] = {500, 500, 500, 1000};
Buzzer_PlayMelody(my_melody, my_durations, 4);
```

## Demo có sẵn

Khi chạy chương trình, hệ thống sẽ tự động thực hiện demo bao gồm:

1. **Startup Sound:** 3 nốt ngắn khi khởi động
2. **Scale Test:** Chơi các nốt từ Do đến Do cao
3. **Alarm Sound:** Tiếng báo động xen kẽ cao-thấp
4. **Police Siren:** Hiệu ứng còi cảnh sát
5. **Melody:** Bài hát "Mary Had a Little Lamb"

Demo sẽ lặp lại mỗi 10 giây.

## Công thức tính tần số PWM

```
PWM_Frequency = Timer_Clock / (ARR + 1)
ARR = (Timer_Clock / Desired_Frequency) - 1

Với Timer_Clock = 1MHz:
ARR = (1000000 / frequency) - 1
```

## Lưu ý quan trọng

1. **Passive vs Active Buzzer:** Code này dành cho Passive Buzzer (cần tín hiệu PWM). Active Buzzer chỉ cần ON/OFF.

2. **Điện áp:** Passive buzzer thường hoạt động ở 5V, nhưng 3.3V cũng có thể dùng được (âm thanh nhỏ hơn).

3. **Transistor:** Bắt buộc phải dùng transistor vì STM32 không thể cấp đủ dòng cho buzzer.

4. **Tần số:** Tần số từ 100Hz đến 10kHz là phù hợp cho buzzer thông thường.

5. **Duty Cycle:** 50% duty cycle cho âm thanh tốt nhất.

## Troubleshooting

### Không có âm thanh
- Kiểm tra kết nối transistor
- Đảm bảo dùng Passive Buzzer
- Kiểm tra nguồn cấp cho buzzer

### Âm thanh quá nhỏ
- Tăng điện áp cấp cho buzzer (5V thay vì 3.3V)
- Kiểm tra transistor có hoạt động đúng không
- Thử transistor khác nếu cần

### Tần số không đúng
- Kiểm tra system clock configuration
- Đảm bảo prescaler được set đúng (95 cho 96MHz clock)
- Verify timer configuration trong STM32CubeMX

## Mở rộng

Có thể mở rộng project này bằng cách:

1. Thêm input từ button để trigger các âm thanh khác nhau
2. Lưu nhiều giai điệu trong flash memory
3. Tạo interface UART để control buzzer từ PC
4. Thêm volume control bằng cách thay đổi duty cycle
5. Tích hợp với sensors để tạo âm báo hiệu

## Tài liệu tham khảo

- [STM32F411 Reference Manual](https://www.st.com/resource/en/reference_manual/dm00119316-stm32f411xc-e-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)
- [DeepBlue Embedded - STM32 Buzzer Tutorial](https://deepbluembedded.com/stm32-buzzer-piezo-active-passive-buzzer-example-code-tone/)
- [STM32 HAL Driver Documentation](https://www.st.com/content/st_com/en/products/embedded-software/mcu-mpu-embedded-software/stm32-embedded-software/stm32cube-mcu-mpu-packages/stm32cubef4.html)
