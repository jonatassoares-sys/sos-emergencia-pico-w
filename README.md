# Sistema Inteligente de Sinalização de Emergência Audiovisual (SOS)

## Universidade Federal do Vale do São Francisco – UNIVASF

**Curso:** Engenharia Elétrica

**Disciplina:** Microcontroladores e Microprocessadores

**Docente:** Prof. Dr. Ricardo Menezes Prates

**Semestre:** 2026

---

## Equipe

- Jônatas de Sousa Soares da Cruz
- Natan Éder Valeriano de Souza

---

# Sobre o Projeto

Este repositório contém o desenvolvimento do projeto **Sistema Inteligente de Sinalização de Emergência Audiovisual (SOS)**, elaborado como atividade avaliativa da Terceira Verificação da disciplina de Microcontroladores e Microprocessadores.

O projeto consiste na implementação de um sistema embarcado capaz de emitir automaticamente o sinal internacional de socorro (SOS) em Código Morse, utilizando sinalização luminosa e sonora sincronizadas. O sistema também realiza o monitoramento contínuo do ambiente por meio de um sensor de ruído, possibilitando o acionamento automático do modo de emergência quando detectadas condições previamente estabelecidas.

O desenvolvimento foi realizado utilizando a plataforma Raspberry Pi Pico W integrada à placa BitDogLab, explorando recursos avançados do microcontrolador RP2040, como conversão analógico-digital (ADC), modulação por largura de pulso (PWM), interrupções externas (IRQ) e temporizadores de hardware.

---

# Objetivos

O projeto tem como objetivo desenvolver um sistema embarcado inteligente capaz de:

- reproduzir automaticamente o Código Morse internacional (SOS);
- monitorar continuamente o ambiente através de um sensor de ruído;
- permitir acionamento manual e automático do sistema;
- sincronizar sinalização visual e sonora;
- utilizar arquitetura baseada em eventos, evitando delays bloqueantes;
- aplicar conceitos de programação em tempo real utilizando o RP2040.

---

# Plataforma Utilizada

## Hardware

- Raspberry Pi Pico W
- Microcontrolador RP2040
- BitDogLab
- Microfone integrado
- LED indicador
- Buzzer piezoelétrico
- Botão de acionamento

## Software

- Linguagem C
- Pico SDK
- Visual Studio Code
- CMake
- Wokwi Simulator

---

# Principais Recursos Implementados

Durante o desenvolvimento foram empregados diversos periféricos do RP2040, dentre eles:

- GPIO (General Purpose Input/Output);
- Conversor Analógico-Digital (ADC);
- Modulação por Largura de Pulso (PWM);
- Temporizadores de Hardware;
- Interrupções Externas (IRQ);
- Máquina de Estados Finitos (FSM).

A arquitetura adotada permite que o sistema permaneça continuamente monitorando o ambiente enquanto executa a sinalização de emergência, garantindo maior responsividade e eficiência quando comparada a soluções baseadas em atrasos bloqueantes.

---

# Funcionamento

O sistema possui dois modos de acionamento:

**Acionamento Manual**

Realizado através do botão da placa, utilizando interrupção de hardware para garantir resposta imediata ao usuário.

**Acionamento Automático**

Realizado pelo monitoramento contínuo do sensor de ruído. Ao identificar um nível sonoro acima do limite configurado, o sistema inicia automaticamente a transmissão do sinal SOS.

Após ativado, o firmware sincroniza o LED e o buzzer para reproduzir o padrão internacional do Código Morse.

---

# Estrutura do Projeto

```
.
├── src/
├── include/
├── lib/
├── docs/
├── CMakeLists.txt
├── pico_sdk_import.cmake
├── wokwi.toml
├── diagram.json
└── README.md
```

---

# Simulação

Antes da implementação física, todo o firmware foi validado no simulador **Wokwi**, permitindo testar o mapeamento dos periféricos, a lógica da máquina de estados e o comportamento dos temporizadores antes da gravação na placa física.

---

# Resultados

Os testes demonstraram o correto funcionamento do sistema, incluindo:

- reprodução precisa do Código Morse;
- sincronização entre LED e buzzer;
- acionamento automático por sensor de ruído;
- acionamento manual via interrupção;
- eliminação de falsos acionamentos através de debounce;
- implementação de rotina de *Clean Exit* para desligamento seguro dos atuadores.

---

# Organização do Repositório

Este repositório contém:

- código-fonte do firmware;
- arquivos de configuração do projeto;
- documentação técnica;
- arquivos de simulação;
- relatório desenvolvido para a disciplina.

---

# Referências

- Raspberry Pi Pico SDK
- RP2040 Datasheet
- Documentação Oficial do Wokwi
- Material disponibilizado na disciplina de Microcontroladores e Microprocessadores

---

# Licença

Este projeto foi desenvolvido exclusivamente para fins acadêmicos como requisito de avaliação da disciplina **Microcontroladores e Microprocessadores**, do curso de Engenharia Elétrica da Universidade Federal do Vale do São Francisco (UNIVASF).
