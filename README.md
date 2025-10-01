# Simulador de Sistema Solar em C++

## Descrição
Este é um simulador 3D do sistema solar desenvolvido em C++ usando a biblioteca OpenGL com FreeGLUT. O código simula planetas, luas, asteroides, meteoritos e poeira cósmica orbitando o Sol, aplicando leis da física como gravidade, colisões e conservação de energia. Ele inclui otimizações como culling de frustum para renderização eficiente, timestep adaptativo para simulação precisa e um menu interativo para navegação rápida entre objetos.

O simulador é interativo: você pode mover a câmera livremente, pausar a simulação, ajustar a constante gravitacional e navegar para planetas específicos. Os corpos celestes têm propriedades como massa, raio, cor, se são gasosos ou sólidos, e calculam pesos gravitacionais em tempo real.

## Funcionalidades Principais
- **Corpos Celestes Simulados**: Inclui o Sol, planetas (Mercúrio, Vênus, Terra, Marte, Júpiter, Saturno, Urano, Netuno, Plutão e outros como Haumea, Makemake, Eris), Lua da Terra, Ceres, cinturão de asteroides (200 asteroides), meteoritos (100) e poeira cósmica (50 partículas).
- **Renderização 3D**: Esferas com iluminação, emissão de luz para o Sol, variações de cor e detalhes ajustados com base na distância da câmera.
- **Interatividade**: Controles de câmera (WASD para movimento, QE para cima/baixo, mouse para olhar), menu (M para alternar), navegação rápida (teclas 1-9 para planetas).
- **Otimização**: Apenas corpos visíveis ou principais são atualizados para melhorar performance; grid espacial para interações gravitacionais.
- **Informações na Tela**: Número de corpos ativos, constante G, energia total do sistema, peso na Terra e controles.

## Como as Leis da Física São Aplicadas
O código implementa várias leis da física clássica para tornar a simulação realista, mas em escala reduzida (distâncias e massas são aproximadas para caber na simulação computacional). Aqui vai um breakdown:

- **Lei da Gravitação Universal de Newton**: 
  - A força gravitacional entre dois corpos é calculada como `F = G * m1 * m2 / r²`, onde `G` é a constante gravitacional (ajustável com + e -).
  - No código, isso é aplicado no loop de `updatePhysics()`, computando acelerações (`ax, ay, az`) para cada par de corpos ativos que estejam próximos (usando `shouldInteract()` para otimizar com grid espacial).
  - Suavização (softening) é adicionada para evitar singularidades quando `r` é zero: `distSq = dx² + dy² + dz² + softening²`.

- **Integração Numérica (Método Leapfrog/Verlet)**:
  - Usado para atualizar posições e velocidades: meia aceleração inicial, atualização de posição, meia aceleração final.
  - Isso melhora a estabilidade e precisão em simulações de N-corpos.

- **Timestep Adaptativo**:
  - Calculado em `calculateAdaptiveTimestep()` baseado na aceleração máxima no sistema: `dt = 0.05 / sqrt(maxAccel)`.
  - Limites: `minDt = 0.0001` e `maxDt = 0.01` para evitar instabilidades.

- **Colisões**:
  - Detectadas em `checkCollisions()` quando a distância entre centros < soma dos raios.
  - Colisões são inelásticas: os corpos se fundem conservando momento linear e massa. O novo raio é calculado como cubo da raiz cúbica da soma dos volumes (aproximação para esferas).
  - Corpos inativos são desativados (`isActive = false`).

- **Conservação de Energia**:
  - Energia cinética + potencial é calculada em `calculateEnergy()` a cada 100 frames.
  - Se o erro de energia > 5%, as velocidades são escaladas para corrigir (mantendo a energia constante, simulando conservação).

- **Outras Considerações Físicas**:
  - Órbitas iniciais seguem velocidades circulares: `v = sqrt(G * M / r)` para órbitas estáveis.
  - Meteoritos têm excentricidades para órbitas elípticas.
  - Peso gravitacional: Calculado como soma de forças em cada corpo (exibido para a Terra).
  - Sem relatividade ou distorção espaço-tempo (foco em mecânica clássica).

Observação: Os valores de massa, raio e distância são escalados arbitrariamente para visualização (ex: Terra a 10 unidades do Sol). Não é uma simulação astrofísica precisa, mas educativa.

## Requisitos
- **Sistema Operacional**: Testado no Linux (Ubuntu/Debian, Fedora, etc.).
- **Dependências**: 
  - Compilador C++ (g++).
  - Bibliotecas: FreeGLUT, OpenGL, GLU (instaladas automaticamente pelo script).
- **Ambiente**: Não requer internet após instalação inicial.

## Como Rodar
1. **Salve o Código**: Coloque o código em um arquivo chamado `main.cpp`.
2. **Script de Execução**: Use o script Python `executo.py` (fornecido separadamente) para automatizar tudo:
   - Ele detecta a distribuição Linux (Ubuntu, Fedora, etc.).
   - Instala dependências (freeglut3-dev, libgl1-mesa-dev, etc.) usando apt, yum ou dnf.
   - Compila o código: `g++ -o simulator main.cpp -lGL -lGLU -lglut -std=c++11`.
   - Executa o simulador automaticamente.
   - Se falhar, tenta alternativas e mostra erros para depuração.

   Comando para rodar:
   ```
   python3 executo.py
   ```

3. **Compilação Manual (se necessário)**:
   ```
   g++ -o simulator main.cpp -lGL -lGLU -lglut -std=c++11
   ./simulator
   ```

4. **Controles**:
   - **Movimento**: W (frente), S (trás), A (esquerda), D (direita), Q (cima), E (baixo).
   - **Olhar**: Mouse para rotacionar.
   - **Outros**: R (reset), +/- (ajustar G), P (pausar), M (menu), 1-9 (navegar para planetas).

## Limitações e Melhorias Futuras
- Performance: Com muitos corpos (ex: 200 asteroides), pode ficar lento em máquinas antigas – otimizado com culling e grid.
- Sem texturas ou anéis (como em Saturno).
- Colisões simples (sem fragmentação).
