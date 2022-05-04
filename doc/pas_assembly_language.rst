Linguagem *assembly*
==================

Um programa em linguagem *assembly* é formado por uma sequência de linhas de texto.
Cada linha contém uma instrução, diretiva ou *label*.


   ``sub	r0, r0, 1``

Uma instrução é formada por uma mnemónica, que identifica a instrução,
seguida dos parâmetros.
No exemplo, ``sub`` é a mnemónica da instrução e ``r0, r0, 1``
são os parâmetros.

O P16 dispõe das instruções listadas na tabela (:numref:`instrucoes_p16`).
A quantidade e o tipo dos parâmetros dependem da instrução em causa
e podem ser registos do processador, constantes numéricas ou símbolos.

As mnemónicas das isntruções e o nome dos registos
podem ser escritos em letras maiúsculas ou minúsculas (*case insensitive*).

   .. table:: Instruções do P16
      :widths: auto
      :name: instrucoes_p16

      +------------------------------+--------------------------------+---------------------------+
      | ``ldr   rd, label``          | ``add     rd, rn, rm``         | ``and  rd, rn, rm``       |
      +------------------------------+--------------------------------+---------------------------+
      | ``pop   rd``                 | ``sub     rd, rn, rm``         | ``orr  rd, rn, rm``       |
      +------------------------------+--------------------------------+---------------------------+
      | ``push  rs``                 | ``adc     rd, rn, rm``         | ``eor  rd, rn, rm``       |
      +------------------------------+--------------------------------+---------------------------+
      | ``ldr   rd, [rn, constant]`` | ``sbc     rd, rn, rm``         | ``mvn  rd, rs``           |
      +------------------------------+--------------------------------+---------------------------+
      | ``ldrb  rd, [rn, constant]`` | ``add     rd, rn, constant``   | ``lsl  rd, rn, constant`` |
      +------------------------------+--------------------------------+---------------------------+
      | ``ldr   rd, [rn, rm]``       | ``sub     rd, rn, constant``   | ``lsr  rd, rn, constant`` |
      +------------------------------+--------------------------------+---------------------------+
      | ``ldrb  rd, [rn, rm]``       | ``cmp     rn, rm``             | ``asr  rd, rn, constant`` |
      +------------------------------+--------------------------------+---------------------------+
      | ``str   rs, [rn, constant]`` | ``bzs/beq label``              | ``ror  rd, rn, constant`` |
      +------------------------------+--------------------------------+---------------------------+
      | ``strb  rs, [rn, constant]`` | ``bzc/bne label``              | ``rrx  rd, rn``           |
      +------------------------------+--------------------------------+---------------------------+
      | ``str   rs, [rn, rm]``       | ``bcs/blo label``              | ``mov  rd, rs``           |
      +------------------------------+--------------------------------+---------------------------+
      | ``strb  rs, [rn, rm]``       | ``bcc/bhs label``              | ``movs pc, lr``           |
      +------------------------------+--------------------------------+---------------------------+
      | ``msr   cpsr, rs``           | ``bge     label``              | ``mov  rd, constant``     |
      +------------------------------+--------------------------------+---------------------------+
      | ``msr   spsr, rs``           | ``blt     label``              | ``movt rd, constant``     |
      +------------------------------+--------------------------------+---------------------------+
      | ``mrs   rd, cpsr``           | ``b       label``              |                           |
      +------------------------------+--------------------------------+---------------------------+
      | ``mrs   rd, spsr``           | ``bl      label``              |                           |
      +------------------------------+--------------------------------+---------------------------+

Se for necessário referenciar a instrução,
por exemplo para saltar para ela desde outro ponto do programa,
precede-se a instrução de uma *label*.

   .. code-block:: console

      cycle:
              sub	r0, r0, 1
              . . .
              b 	cycle

A *label* define um símbolo. No exemplo acima é definido o símbolo ``cycle``,
cujo valor é o endereço de memória onde está alojado o código máquina da instrução ``sub r0, r0, 1``.

Sintacticamente, é formada por uma palavra iniciada por uma letra
seguida de mais letras ou dígitos e terminada pelo carácter ``‘:’``.
Pode conter o carácter ``‘_’``, tanto no início da palavra, como entre caracteres.
O compilador distingue letras maiúsculas de minúsculas
na definição de símbolo do tipo *label* (*case sensitive*).
Para melhor evidência, a *label* costuma colocar-se
na linha anterior à da instrução a que se refere.

Os comentários podem ser inseridos em qualquer lugar
quando delimitados pelas marcas ``‘/*’`` e ``‘*/’`` como em linguagem C,
ou depois do carácter ``‘;’`` até ao fim da linha.

  .. code-block:: console

      /* Main cycle.
	 Iterates until counter reach zero.
      */
      cycle:
              sub	r0, r0, 1	; decrement counter
              . . .
              b 	cycle

Diretivas
---------

Directivas de compilação são comandos que permitem ao programador controlar
a operação do *assembler*.

Sintacticamente uma directiva é identificada por uma palavra chave iniciada pelo carácter '.'.
No texto do programa, uma directiva e os seus parâmetros ocupam
a mesma posição da mnemónica da instrução e dos respectivos parâmetros.

Na linguagem *assembly* do P16 existem directivas para definir os dados do programa,
para controlar a localização dos dados e do código máquina em memória e para definir símbolos.

.. table:: Directivas para definição de dados do programa
  :widths: 8 12
  :name: diretiva_dados

  +---------------------------------------------+--------------------------------------------------------------+
  | ``.byte`` [*expr1* [, *expr2*, ...]]        | Reserva uma sequência de *bytes*, cada um inicializado       |
  |                                             | com o valor da respetiva expressão.                          |
  |                                             | Se não forem indicados parâmetros, reserva um *byte*         |
  |                                             | inicializado com zero.                                       |
  +---------------------------------------------+--------------------------------------------------------------+
  | ``.word`` [*expr1* [, *expr2*, ...]]        | Reserva uma sequência de *words*, cada uma inicializada      |
  |                                             | com o valor da respetiva expressão.                          |
  |                                             | Se não forem indicados parâmetros, reserva uma *word*        |
  |                                             | inicializada com zero.                                       |
  +---------------------------------------------+--------------------------------------------------------------+
  | ``.space``  *size* [, *fill* ]              | Reserva um bloco de memória com a dimensão em *bytes*        |
  |                                             | indicada pelo parâmetro *size*,                              |
  |                                             | sendo cada *byte* inicializado com o valor da expressão      |
  |                                             | *fill*. Se o parâmetro *fill* for omitido                    |
  |                                             | o bloco será preenchido com zeros.                           |
  +---------------------------------------------+--------------------------------------------------------------+
  | ``.ascii`` *“string1”* [, *“string2”*, ...] | Reserva uma porção de memória para alojar a sequência        |
  |                                             | de *strings* indicada. As *strings* produzidas são compostas |
  |                                             | apenas pelos caracteres indicados, não contêm terminador.    |
  +---------------------------------------------+--------------------------------------------------------------+
  | ``.asciz`` *“string1”* [, *“string2”*, ...] | Reserva uma porção de memória para alojar a sequência        |
  |                                             | de *strings* indicada. As *strings* produzidas são compostas |
  |                                             | pelos caracteres indicados e terminadas com o valor zero.    |
  +---------------------------------------------+--------------------------------------------------------------+
  | ``.align`` [n]                              | Avança o contador de localização até um valor múltiplo de    |
  |                                             | 2^n. O novo valor do contador terá zero nos 'n' *bits*       |
  |                                             | de menor peso. A omissão de argumento é equivalente a        |
  |                                             | ``.align 1``.                                                |
  +---------------------------------------------+--------------------------------------------------------------+

Exemplo de utilização da directiva ``.word`` na definição de uma variável de 16 *bits*
identificada pelo símbolo ``counter`` iniciada com o valor zero.

   .. code-block::

      counter:
      	.word	0

Exemplo de utilização da directiva ``.byte na definição de um *array*
de valores representados a 8 *bits*, com três posições,
iniciadas com os valores 3, 4 e 5, sucessivamente.

   .. code-block::

      array:
      	.byte	3, 4, 5

Exemplo de utilização da directiva ``.asciz`` para definição de um *array*
de caracteres iniciado com a *string* “Portugal”, no formato da linguagem C.
Neste formato cada posição do *array* guarda o código de um carácter,
começando no endereço mais baixo e pela ordem de escrita.
A terminação da *string* é assinalada com o valor zero
na posição a seguir à do último carácter.
Neste exemplo são ocupadas nove posições de memória,
oito para os códigos dos caracteres e uma para o terminador.

.. code-block::

   message:
   	.asciz	“Portugal”

.. table:: Directivas para definição de secções
   :widths: 8 12
   :name: diretiva_seccoes

   +----------------------------+-----------------------------------------------+
   | ``.section`` *name*        | Define uma secção, designada por *name*.      |
   +----------------------------+-----------------------------------------------+
   | ``.text``                  | Define uma secção com o nome ``.text``.       |
   +----------------------------+-----------------------------------------------+
   | ``.rodata``                | Define uma secção com o nome '.rodata'.       |
   +----------------------------+-----------------------------------------------+
   | ``.data``                  | Define uma secção com o nome '.data'.         |
   +----------------------------+-----------------------------------------------+
   | ``.bss``                   | Define uma secção com o nome '.bss'.          |
   +----------------------------+-----------------------------------------------+

.. table:: Directiva para definição de símbolos
   :widths: 6 14
   :name: diretiva_simbolos

   +----------------------------+-----------------------------------------------+
   | ``.equ`` *name*, *value*   | Define o símbolo *name* como sendo            |
   |                            | equivalente a *value*.                        |
   +----------------------------+-----------------------------------------------+

Símbolos
--------

Existem duas formas de definir símbolos: através de *labels*
ou através da directiva ``.equ``.
No caso das *labels* o símbolo é equivalente ao endereço da instrução
ou da variável seguinte.
No caso da directiva ``.equ`` o símbolo é equivalente ao valor da constante
ou expressão associada.

.. code-block::

   	.equ	MASK, 0b00001110

Neste exemplo, a directiva ``.equ`` é usada para definir o símbolo ``MASK``
como equivalente ao valor binário ``1110``.

Contador de localização
-----------------------

O contador de localização é uma variável interna do *assembler*
e contém o endereço onde o código da instrução corrente pode eventualmente
ser carregado em memória.
Quando a tradução do programa começa, esta variável é inicializada a zero.
À medida que as instruções ou directivas são processadas,
o contador de localização é aumentado da dimensão necessária para armazenar
o código máquina da instrução ou o conteúdo da variável.

Existe um contador de localização para cada secção.

A linguagem *assembly* do P16 usa o símbolo ``'.'`` (um ponto isolado)
como identificador do contador de localização.
No contexto da linguagem *assembly*,
este símbolo substitui a *label* referente à instrução ou directiva corrente.

.. code-block::

   	b	.

No exemplo, a instrução *branch* realiza um salto para
a posição onde ela própria se encontra,
confinando o processamento à execução repetida desta instrução.

Secções
-------

As secções são zonas de memória que alojam elementos do programa
-- o código de instruções ou os dados do programa,
segundo critérios de afinidade.
O caso mais simples consiste em agrupar o código das instruções numa secção
e as variáveis noutra secção.

Antes de especificar qualquer instrução ou directiva
deve-se definir a secção que as vai conter.
A secção corrente é definida pela directiva ``.section``
ou pelas directivas especificas ``.text``. ``.rodata``, ``.data`` ou ``.bss``.

O programa seguinte é composto pela secção ``.data``
onde se alojam as variáveis ``x``e ``y``,
pela secção ``.bss`` onde se aloja a variável ``z``
e pela secção ``.text`` onde se aloja o código máquina do programa.
A secção ``.data`` está localizada no endereço ``0x20a0`` e tem dimensão quatro.
A secção ``.bss`` estaá localizada no endereço ``0x20a4`` e tem dimensão dois.
A secção ``.text`` está localizada no endereço ``0xb000`` e tem a dimensão 22 (0x16).
Os valores dos endereços usados neste exemplo são arbitrários.
Conforme veremos mais adiante,
os endereços das secções são atribuídos em fase posterior à da escrita do programa
(Secção 16.4.1).

.. literalinclude:: /code/sections/seccoes1.lst
   :language: asm
   :caption: Exemplo de utilização das secções ``.text``, ``.data`` e ``.bss``
   :name: ficheiro_seccoes1

Uma secção pode ser fragmentada ao longo do texto do programa.
Por exemplo, para que as variáveis possam ser definidas
junto ao código das funções que as utilizam,
mas alojadas na secção ``.data``,
haverá fragmentos de ``.text`` entrecortados por fragmentos de ``.data``.
Durante a compilação, os fragmentos de uma secção são concatenados,
pela ordem em que aparecem ao longo da descrição do programa para formar
a composição final de cada uma das secções.

.. literalinclude:: /code/sections/seccoes2.lst
   :language: asm
   :caption: Exemplo de secções entrecortadas
   :name: ficheiro_seccoes2

Neste exemplo, mostram-se os endereços calculados de forma coerente,
embora tendo por base valores novamente arbitrários.
A variável ``ptr`` é alojada na secção ``.data``
e a sua definição surge junto da função ``strtok`` que a utiliza.
O mesmo acontece com a variável ``counter`` e a função ``accumulate``.
A secção ``text`` está separada em dois fragmentos
o primeiro contém o código da função ``strtok``
e o segundo contém o código da função ``accumulate``.
Na memória as duas variáveis ocupam posições contíguas
-- ``ptr`` ocupa as posições de endereço ``0x1000`` e ``0x1001``
e ``counter`` a posição de endereço ``0x1002``.
O código das funções ``strtok`` e ``accumulate`` ocupam também zonas de memória contíguas,
respectivamente, a gama de endereços ``0x3000`` a ``0x3007``
e a gama de endereços ``0x3008`` a ``0x300f``.

Limitações sintáticas
---------------------

* Não é possível definir símbolos iguais a mnemónicas de instruções.
  Por exemplo, não pode existir um símbolo “b”
  porque coincide com a mnemónica da instrução *branch*.

* A última linha do programa também deve ser terminada com "mudança de linha".
  Se o ficheiro do programa terminar numa linha incompleta é assinalado erro de sintaxe.
