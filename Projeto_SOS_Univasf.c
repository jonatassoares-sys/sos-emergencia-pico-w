#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"

// ================= Definições de Pinos (Padrão BitDogLab) =================
#define LED_PIN 11       // LED de alerta 
#define BUZZER_PIN 21    // Buzzer piezoelétrico
#define BUTTON_PIN 5     // Botão A
#define MIC_PIN 28       // Microfone nativo conectado ao ADC2 (GPIO 28)

// ================= Variáveis Globais Voláteis =================
volatile bool modo_emergencia = false;
volatile int tempo_morse = 0;
volatile int estado_morse = 0; 
volatile uint32_t ultimo_tempo_botao = 0;

// AVISO PARA O COMPILADOR: A função existe e está lá embaixo!
void acionar_alerta(bool ligar);

// ================= Função de Interrupção (IRQ) do Botão =================
void botao_callback(uint gpio, uint32_t events) {
    // Pega o tempo atual em milissegundos desde que a placa ligou
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    
    // Filtro Debounce: Só aceita um clique se passou pelo menos 250ms do último
    if (tempo_atual - ultimo_tempo_botao > 250) {
        ultimo_tempo_botao = tempo_atual;
        
        modo_emergencia = !modo_emergencia;
        estado_morse = 0;
        tempo_morse = 0;
        
        // A SOLUÇÃO: Força o hardware a desligar imediatamente 
        // caso o sistema esteja saindo do modo de emergência.
        if (!modo_emergencia) {
            acionar_alerta(false);
        }
    }
}

// ================= Configuração de PWM (LED e Buzzer) =================
void configurar_pwm(uint pino, uint wrap, float duty_cycle) {
    gpio_set_function(pino, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pino);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(pino), (uint16_t)(wrap * duty_cycle));
    pwm_set_enabled(slice_num, false); // Começa desligado
}

void acionar_alerta(bool ligar) {
    uint slice_led = pwm_gpio_to_slice_num(LED_PIN);
    uint slice_buzzer = pwm_gpio_to_slice_num(BUZZER_PIN);
    
    if (ligar) {
        // 1. Liga os motores do PWM
        pwm_set_enabled(slice_led, true);
        pwm_set_enabled(slice_buzzer, true);
        
        // 2. Devolve os pinos para o controle do PWM
        gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
        gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    } else {
        // 1. A SOLUÇÃO BRUTA: Arranca o controle do PWM e vira pino digital comum
        gpio_init(LED_PIN);
        gpio_set_dir(LED_PIN, GPIO_OUT);
        gpio_put(LED_PIN, 0); // Força 0V na mesma hora!
        
        gpio_init(BUZZER_PIN);
        gpio_set_dir(BUZZER_PIN, GPIO_OUT);
        gpio_put(BUZZER_PIN, 0); // Força 0V na mesma hora!
        
        // 2. Agora sim podemos desligar o motor do PWM nos bastidores
        pwm_set_enabled(slice_led, false);
        pwm_set_enabled(slice_buzzer, false);
    }
}

// ================= Máquina de Estados Morse (SOS) =================
// 1 = Ligado, 0 = Desligado. Cada número representa 200ms de tempo.
int padrao_sos[] = {
    1,0, 1,0, 1,              // Letra S (...) 
    0,0,0,                    // Pausa entre letras
    1,1,1,0, 1,1,1,0, 1,1,1,  // Letra O (---)
    0,0,0,                    // Pausa entre letras
    1,0, 1,0, 1,              // Letra S (...)
    0,0,0,0,0,0,0             // Pausa longa para reiniciar o ciclo
};

// Calcula o tamanho do array automaticamente
int tamanho_sos = sizeof(padrao_sos) / sizeof(padrao_sos[0]);

// Função chamada automaticamente pelo hardware
bool timer_morse_callback(struct repeating_timer *t) {
    if (!modo_emergencia) {
        acionar_alerta(false);
        return true; 
    }

    // Lê a "partitura" do array para saber se liga ou desliga
    if (padrao_sos[tempo_morse] == 1) {
        acionar_alerta(true); 
    } else {
        acionar_alerta(false); 
    }
    
    // Avança para o próximo tempo
    tempo_morse++;
    if(tempo_morse >= tamanho_sos) {
        tempo_morse = 0; // Se chegou no fim, reinicia o SOS
    }

    return true; 
}

// ================= Main =================
int main() {
    stdio_init_all();

    // 1. Configurar ADC (Sensor de Som/Microfone)
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(2); // GPIO 28 é o canal 2 do ADC

    // 2. Configurar PWM (LED e Buzzer)
    // LED: Frequência normal, 50% duty cycle
    configurar_pwm(LED_PIN, 4095, 0.5); 
    // Buzzer: Wrap define a frequência (tom). Ajustem para um tom agradável.
    configurar_pwm(BUZZER_PIN, 2000, 0.5); 

    // 3. Configurar Botão com Interrupção (IRQ)
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &botao_callback);

    // 4. Configurar Timer de Hardware
    struct repeating_timer timer;
    // Chama a função a cada 200ms
    add_repeating_timer_ms(200, timer_morse_callback, NULL, &timer);

    // 5. Loop Principal 
    while (true) {
        uint16_t leitura_mic = adc_read();
        
        // Remova ou comente o printf se o monitor ficar muito rápido
        printf("Nivel de Som: %d | Emergencia: %s\n", leitura_mic, modo_emergencia ? "ON" : "OFF");
        
        // Gatilho sonoro: Captura picos de ruído acima de 2800 ou abaixo de 1200
        if ((leitura_mic > 3800 || leitura_mic < 200) && !modo_emergencia) {
            modo_emergencia = true;
            estado_morse = 0;
            tempo_morse = 0;
        }
        
        // Sleep reduzido de 100 para 10 milissegundos (Placa escuta 100 vezes por segundo)
        sleep_ms(10);
    }
}