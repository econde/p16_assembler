Linguagem *assembly*
====================

Um programa em linguagem *assembly* é formado por uma sequência de linhas de texto.
Cada linha contém uma instrução, diretiva ou *label*.

Uma instrução é formada por uma mnemónica, que identifica a instrução,
seguida dos parâmetros.
No exemplo, ``sub`` é a mnemónica da instrução e ``r0, r0, 1``
são os parâmetros.

   ``sub	r0, r0, 1``

O P16 dispõe das instruções listadas na tabela (:numref:`instrucoes_p16`)
:ref:`[2]<ref2>`.
A quantidade e o tipo dos parâmetros dependem da instrução em causa
e podem ser registos do processador, constantes numéricas ou símbolos.

As mnemónicas das instruções e o nome dos registos
podem ser escritos em letras maiúsculas ou minúsculas (*case insensitive* [#f1]_).

   .. table:: Instruções do P16
      :widths: auto
      :name: instrucoes_p16

      +-------------------------------+---------------------------------+----------------------------+
      | ``ldr   rd, label``           | ``add     rd, rn, rm``          | ``and  rd, rn, rm``        |
      +-------------------------------+---------------------------------+----------------------------+
      | ``pop   rd``                  | ``sub     rd, rn, rm``          | ``orr  rd, rn, rm``        |
      +-------------------------------+---------------------------------+----------------------------+
      | ``push  rs``                  | ``adc     rd, rn, rm``          | ``eor  rd, rn, rm``        |
      +-------------------------------+---------------------------------+----------------------------+
      | ``ldr   rd, [rn, #constant]`` | ``sbc     rd, rn, rm``          | ``mvn  rd, rs``            |
      +-------------------------------+---------------------------------+----------------------------+
      | ``ldrb  rd, [rn, #constant]`` | ``add     rd, rn, #constant``   | ``lsl  rd, rn, #constant`` |
      +-------------------------------+---------------------------------+----------------------------+
      | ``ldr   rd, [rn, rm]``        | ``sub     rd, rn, #constant``   | ``lsr  rd, rn, #constant`` |
      +-------------------------------+---------------------------------+----------------------------+
      | ``ldrb  rd, [rn, rm]``        | ``cmp     rn, rm``              | ``asr  rd, rn, #constant`` |
      +-------------------------------+---------------------------------+----------------------------+
      | ``str   rs, [rn, #constant]`` | ``bzs/beq label``               | ``ror  rd, rn, #constant`` |
      +-------------------------------+---------------------------------+----------------------------+
      | ``strb  rs, [rn, #constant]`` | ``bzc/bne label``               | ``rrx  rd, rn``            |
      +-------------------------------+---------------------------------+----------------------------+
      | ``str   rs, [rn, rm]``        | ``bcs/blo label``               | ``mov  rd, rs``            |
      +-------------------------------+---------------------------------+----------------------------+
      | ``strb  rs, [rn, rm]``        | ``bcc/bhs label``               | ``movs pc, lr``            |
      +-------------------------------+---------------------------------+----------------------------+
      | ``msr   cpsr, rs``            | ``bge     label``               | ``mov  rd, #constant``     |
      +-------------------------------+---------------------------------+----------------------------+
      | ``msr   spsr, rs``            | ``blt     label``               | ``movt rd, #constant``     |
      +-------------------------------+---------------------------------+----------------------------+
      | ``mrs   rd, cpsr``            | ``b       label``               |                            |
      +-------------------------------+---------------------------------+----------------------------+
      | ``mrs   rd, spsr``            | ``bl      label``               |                            |
      +-------------------------------+---------------------------------+----------------------------+

.. rubric:: *Label*

Se for necessário referenciar uma instrução ou variável,
precede-se essa instrução ou variável de uma *label*.

.. code-block:: console

   cycle:
           sub	r0, r0, #1
           . . .
           b 	cycle

No exemplo acima, precede-se a instrução ``sub  r0, r0, 1`` da label ``cycle:``
para indicar o local para onde a instrução ``b  cycle`` deve "saltar"
para iniciar novo ciclo.

.. rubric:: Comentários

Os comentários podem ser inseridos em qualquer lugar
quando delimitados pelas marcas ``'/*'`` e ``'*/'`` como em linguagem C,
ou depois do carácter ``';'`` até ao fim da linha.

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

Directivas de compilação são comandos que permitem controlar
a operação do *assembler*.

Sintacticamente uma directiva é identificada por uma palavra chave iniciada pelo carácter \'.\'.
No texto do programa, uma directiva e os seus parâmetros ocupam
a mesma posição da mnemónica da instrução e dos respectivos parâmetros.

Na linguagem *assembly* do P16 existem directivas para definir dados do programa,
para controlar a localização dos dados e do código máquina em memória e para definir símbolos.

.. rubric::  Directivas para definição de dados do programa

.. table:: Directivas para definição de dados do programa
  :widths: 7 12
  :name: diretiva_dados

  +-------------------------------------------------+-------------------------------------------------------------------+
  | ``.byte`` [*expr1* [, *expr2*, ...]]            | Reserva uma sequência de *bytes*, cada um inicializado            |
  |                                                 | com o valor da respetiva expressão. Sem argumento não             |
  |                                                 | produz efeito.                                                    |
  +-------------------------------------------------+-------------------------------------------------------------------+
  | ``.word`` [*expr1* [, *expr2*, ...]]            | Reserva uma sequência de *words*, cada uma inicializada           |
  |                                                 | com o valor da respetiva expressão.  Sem argumento não            |
  |                                                 | produz efeito.                                                    |
  |                                                 | Uma *word* é composta por dois *bytes*, sendo o de menor peso     |
  |                                                 | colocado na posição de menor endereço                             |
  |                                                 | que deve ser um endereço par.                                     |
  +-------------------------------------------------+-------------------------------------------------------------------+
  | ``.space``  *size* [, *fill* ]                  | Reserva um bloco de memória com a dimensão em *bytes*             |
  |                                                 | indicada pelo parâmetro *size*,                                   |
  |                                                 | sendo cada *byte* inicializado com o valor da expressão           |
  |                                                 | *fill*. Se o argumento *fill* for omitido                         |
  |                                                 | o bloco será preenchido com      zeros.                           |
  +-------------------------------------------------+-------------------------------------------------------------------+
  | ``.ascii`` *\"string1\"* [, *\"string2\"*, ...] | Reserva uma porção de memória para alojar a sequência             |
  |                                                 | de *strings* definida. As *strings* produzidas são compostas      |
  |                                                 | apenas pelos caracteres indicados, não contêm terminador.         |
  +-------------------------------------------------+-------------------------------------------------------------------+
  | ``.asciz`` *\"string1\"* [, *\"string2\"*, ...] | Reserva uma porção de memória para alojar a sequência             |
  |                                                 | de *strings* definida. As *strings* produzidas são compostas      |
  |                                                 | pelos caracteres indicados e terminadas com o valor zero          |
  |                                                 | (como na linguagem C).                                            |
  +-------------------------------------------------+-------------------------------------------------------------------+
  | ``.align`` [*n*]                                | Avança o contador de localização até um valor múltiplo de         |
  |                                                 | 2^n. O novo valor do contador terá zero nos *n* *bits*            |
  |                                                 | de menor peso. A omissão de argumento é equivalente a             |
  |                                                 | ``.align 1``.                                                     |
  +-------------------------------------------------+-------------------------------------------------------------------+

Exemplo de utilização da directiva ``.word`` na definição de uma variável de 16 *bits*
inicializada com o valor mil. Este valor é representado a 16 *bits* (0000 0011 1110 1000),
sendo o *byte* de menor peso guardado na posição de memória de endereço menor
e o *byte* de maior peso guardado na posição de endereço maior.

.. code-block:: console

   counter:
   	.word	1000

Exemplo de utilização da directiva ``.byte`` na definição de um *array*
de três posições iniciadas com 3, 4 e 5, sucessivamente. São ocupadas três
posições de memória. A posição de endereço mais baixo recebe o valor 3,
a seguinte o valor 4 e a última o valor 5.

.. code-block:: console

   array:
   	.byte	3, 4, 5

Exemplo de utilização da directiva ``.asciz`` para definição de um *array*
de caracteres iniciado com a *string* \"Portugal\", no formato da linguagem C.
Neste formato cada posição do *array* guarda o código de um carácter,
começando no endereço mais baixo e pela ordem de escrita.
A terminação da *string* é assinalada com o valor zero
na posição a seguir à do último carácter.
Neste exemplo são ocupadas nove posições de memória,
oito para os códigos dos caracteres e uma para o terminador.

.. code-block:: console

   message:
   	.asciz	"Portugal"

.. rubric::  Directivas para definição de secções

.. table:: Directivas para definição de secções
   :widths: 8 12
   :name: diretiva_seccoes

   +----------------------------+-----------------------------------------------+
   | ``.section`` *name*        | Define uma secção, designada por *name*.      |
   +----------------------------+-----------------------------------------------+
   | ``.text``                  | Define uma secção com o nome ``.text``.       |
   +----------------------------+-----------------------------------------------+
   | ``.rodata``                | Define uma secção com o nome ``.rodata``.     |
   +----------------------------+-----------------------------------------------+
   | ``.data``                  | Define uma secção com o nome ``.data``.       |
   +----------------------------+-----------------------------------------------+
   | ``.bss``                   | Define uma secção com o nome ``.bss``.        |
   +----------------------------+-----------------------------------------------+
   | ``.stack``                 | Define uma secção com o nome ``.stack``.      |
   +----------------------------+-----------------------------------------------+

A inserção de uma diretiva de definição de secção no texto do programa,
significa que todos os elementos de programa, instruções ou variáveis,
definidos depois dessa diretiva, são alojados nessa secção.

.. rubric::  Directivas para definição de símbolos

.. table:: Directiva para definição de símbolos
   :widths: 6 14
   :name: diretiva_simbolos

   +----------------------------+-----------------------------------------------+
   | ``.equ`` *name*, *value*   | Define o símbolo *name* como sendo            |
   |                            | equivalente a *value*.                        |
   +----------------------------+-----------------------------------------------+

A diretiva ``.equ`` permite definir um símbolo e associar-lhe um valor.
Esse valor pode ser expresso na forma de um número,
outro símbolo também definido com ``.equ``, uma *label*
ou uma expressão envolvendo qualquer um dos anteriores.
O valor deve ser calculável pelo *assembler*, isto é,
não pode envolver simbolos que não estejam definidos.

Esta diretiva pode ser posicionada em qualquer lugar do texto do programa,
tanto antes como depois da invocação do respetivo símbolo.

Símbolos
--------

Um símbolo é uma palavra iniciada por uma letra seguida de mais letras ou dígitos.
Pode conter o carácter ``'_'``, tanto no início da palavra, como entre caracteres.
A formação de símbolos é *case sensitive* [#f1]_.

Os símbolos podem ser definidos através de *labels* ou da diretiva ``.equ``.

.. rubric:: *label*

Uma *label* é formada pelo símbolo seguido do carácter ``':'``.
Na invocação de uma *label* utiliza-se apenas o símbolo sem o carácter ``':'``.
Um símbolo do tipo *label* é equivalente ao endereço de memória
onde o código binário da instrução ou o valor da variável que lhe sucede está alojado.

No exemplo seguinte, a *label* ``counter:`` define o símbolo ``counter``,
cujo valor associado é o endereço de memória onde está alojado o conteúdo
da variável.

.. code-block:: console

   counter:
   	.word	0

Para melhor evidência, a *label* costuma colocar-se
na linha anterior à da instrução ou variável a que se refere.

.. rubric:: .equ

..
   Um símbolo definido com a diretiva ``.equ``
   tem um domínio de valores na gama :math:`-2^{16 - 1}` e :math:`+2^{16 - 1} - 1`.

No exemplo seguinte, a directiva ``.equ`` define o símbolo ``MODE_MASK``
equivalente ao valor binário ``1110``.

.. code-block:: console

   	.equ	MODE_MASK, 0b00001110

Em geral, na composição léxica de símbolos usam-se as seguintes convenções:
   - na *label*, letras minúsculas
     e o carácter ``'_'`` na separação de palavras em símbolos compostos.

   - na diretiva ``.equ``, letras maiúsculas
     e o carácter ``'_'`` na separação de palavras em símbolos compostos.

Expressões
----------
Os parâmetros constantes das instruções ou diretivas são definidos através de expressões.
Estas podem ser valores numéricos simples ou expressões envolvendo vários operadores.

O valor de uma expressão ou sub-expressão é um número natural (conjunto N)
no domínio :math:`0` a :math:`2^{16} - 1`.

Exemplos de expressões: ::

   mov    r0, #3
   movt   r0, #((1 << 13) >> 8)

   .word  1000 / 2 + 56

As expressões podem conter símbolos definidos com a diretiva ``.equ``
com base noutra expressão.

Exemplo de expressão com símbolos: ::

   .equ   MASK, 1 << 4
   .equ   VALUE, 2055

   mov    r0, #VALUE & MASK

No cálculo das expressões, o resultado de cada operação é avaliado.
Se a sua magnitude for inferior a :math:`2^{16}` (não codificável a 16 *bits*),
é emitida uma mensagem de aviso.

No caso da subtração ou da operação unária negativo,
um suposto resultado negativo com magnitude inferior a :math:`2^{16}`
é tratado como um resultado correto.

Por exemplo a expressão **-1** é codificada da mesma forma que a expressão **0xffff**.

No seguinte exemplo, ambas as instruções carregam o valor **Oxff** em R0.

::

   mov	r0, #-1 >> 8
   mov	r0, #0xffff >> 8

Operadores aritméticos
^^^^^^^^^^^^^^^^^^^^^^

.. table:: Operadores aritméticos
   :widths: 8 12
   :name: operadores_aritmeticos

   +------------+----+--------------------------+
   | Binários   | \+ | adição                   |
   |            +----+--------------------------+
   |            | \- | subtração                |
   |            +----+--------------------------+
   |            | \* | multiplicação            |
   |            +----+--------------------------+
   |            | \/ | divisão                  |
   |            +----+--------------------------+
   |            | \% | resto da divisão inteira |
   +------------+----+--------------------------+
   | Unários    | \+ | valor positivo           |
   |            +----+--------------------------+
   |            | \- | valor negativo           |
   +------------+----+--------------------------+

Operadores relacionais
^^^^^^^^^^^^^^^^^^^^^^
.. table:: Operadores relacionais
   :widths: 8 12
   :name: operadores_relacionais
   
   +------------+-----+--------------------------+
   | Ordem      | \<  | menor que                |
   |            +-----+--------------------------+
   |            | \<= | menor ou igual que       |
   |            +-----+--------------------------+
   |            | \>  | maior que                |
   |            +-----+--------------------------+
   |            | \>= | maior ou igual que       |
   +------------+-----+--------------------------+
   | Igualdade  | \== | igual a                  |
   |            +-----+--------------------------+
   |            | \!= | diferente de             |
   +------------+-----+--------------------------+

Os operadores relacionais produzem valores booleanos.
O valor booleando **verdadeiro** é representado como o valor inteiro **1**,
e o valor booleando **falso** é representado como o valor inteiro **0**.

Na seguinte instrução, o registo destino (R0) é afetado com o valor 1. ::

	mov    r0, #3 == (5 - 2)
	

Operadores lógicos
^^^^^^^^^^^^^^^^^^
.. table:: Operadores lógicos
   :widths: 8 12
   :name: operadores_lógicos
   
   +------------+-----+--------------------------+
   | Binários   | \&& | conjunção                |
   |            +-----+--------------------------+
   |            | \|| | disjunção                |
   +------------+-----+--------------------------+
   | Unário     | \!  | negação                  |
   +------------+-----+--------------------------+
Nestas operações os operandos são valores numéricos encarados como valores booleanos.
Um valor numérico zero é encarado como **falso**
e um valor numérico diferente de zero é encarado como **verdadeiro**.

Na seguinte instrução, o registo destino (R0) é afetado com o valor 1.
Ambos os valores, 4 e 2,
são encarados como valores **booleanos** verdadeiro,
por serem diferentes de zero::

	mov    r0, #4 && 2


Operadores *bit-a-bit*
^^^^^^^^^^^^^^^^^^^^^^
.. table:: Operadores *bit-a-bit*
   :widths: 8 12
   :name: operadores_bit_a_bit
   
   +------------+-----+-----------------------------+
   | Binários   | \&  | conjunção (*and*)           |
   |            +-----+-----------------------------+
   |            | \|  | disjunção (*or*)            |
   |            +-----+-----------------------------+
   |            | \^  | disjunção exclusiva (*xor*) |
   +------------+-----+-----------------------------+
   | Unário     | \~  | negação                     |
   +------------+-----+-----------------------------+

Nestas operações, cada *bit* de um operando
é operado com o *bit* da mesma posição do outro operando.

Na seguinte instrução, o registo destino (R0) é afetado com o valor 6. ::

	mov    r0, #7 & 0b1110

Operadores deslocamento
^^^^^^^^^^^^^^^^^^^^^^^
.. table:: Operadores deslocamento
   :widths: 8 12
   :name: operadores_deslocamento
   
   +------+-----------------------------+
   | \<<  | deslocar para a esquerda    |
   +------+-----------------------------+
   | \>>  | deslocar para a direita     |
   +------+-----------------------------+

A operação \<< insere zero na posição de menor peso do resultado.

Como os valores são encarados como números naturais,
a operação \>> insere sempre zero na posição de maior peso do resultado.

Operador condicional
^^^^^^^^^^^^^^^^^^^^
Este operador tem o mesmo significado que na linguagem C. ::

   condition_expression ? true_expression : false_expression 

O valor produzido por este operador é igual ao da expressão ``true_expression``,
se o valor de ``condition_expression`` for avaliado como verdadeiro
ou é igual ao da expressão ``false_expression``
se o valor de ``condition_expression`` for avaliado como falso. 

Contador de localização
-----------------------
Existe um contador de localização, associado a cada secção,
que é inicializado a zero.

À medida que as instruções ou directivas são processadas,
o contador de localização é aumentado da dimensão de memória
necessária para armazenar o código máquina da instrução ou o conteúdo da variável.

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
ou pelas directivas especificas ``.text``. ``.rodata``, ``.data``, ``.bss`` ou ``.stack``.

O programa da :numref:`ficheiro_seccoes1` é composto pela secção ``.data`` (linha 1)
onde se alojam as variáveis ``x``, ``y`` e ``z``,
especificadas pela diretivas que se seguem,
e pela secção ``.text`` (linha 9) onde se aloja o código máquina das instruções que se seguem.
A secção ``.data`` está localizada no endereço ``0x1000`` e tem dimensão de seis *bytes*.
A secção ``.text`` está localizada no endereço ``0x4000`` e tem a dimensão de 22 (0x16) *bytes*.
Os valores dos endereços usados neste exemplo são arbitrários.
Conforme se pode ver na :numref:`Localizacao_seccoes`,
os endereços das secções são atribuídos em fase posterior à da escrita do programa.

.. literalinclude:: /code/sections/seccoes1.lst
   :caption: Exemplo de utilização das secções ``.text``, ``.data`` e ``.bss``
   :name: ficheiro_seccoes1

.. rubric:: Fragmentação

Uma secção pode ser fragmentada ao longo do texto do programa.
Por exemplo, para que as variáveis possam ser definidas
junto ao código das funções que as utilizam,
mas alojadas na secção ``.data``,
haverá fragmentos de ``.text`` entrecortados por fragmentos de ``.data``.
Durante a compilação, os fragmentos de uma secção são concatenados,
pela ordem em que aparecem ao longo da descrição do programa para formar
a composição final de cada uma das secções.

.. literalinclude:: /code/sections/seccoes2.lst
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

Regras sintáticas
-----------------

A linguagem *assembly* do P16 é semelhante à usada pelo assembler AS da GNU
quando usado no desenvolvimento de programas para a arquitectura ARM.
O objectivo é facilitar ao estudante a transição para essa arquitectura.
Na especificação de constantes a utilização de ``#`` é opcional,
tal como na sintaxe unificada da arquitectura ARM.

.. table:: Elementos da notação *Wirth Syntax Notation* (WSN)
   :name: notacao WSN

   +-----------+-------------------------------------------------------------------+
   | ``[a]``   | O elemento ``a`` é opcional.                                      |
   +-----------+-------------------------------------------------------------------+
   | ``a | b`` | ``a`` ou ``b`` são elementos alternativos                         |
   +-----------+-------------------------------------------------------------------+
   | ``{a}``   | O elemento ``a`` pode não existir ou repetir-se indefinidamente   |
   +-----------+-------------------------------------------------------------------+
   | ``"a"``   | Elemento terminal                                                 |
   +-----------+-------------------------------------------------------------------+

Descrevem-se na :numref:`sintaxe`,
em notação *Wirth Syntax Notation* (WSN) :numref:`notacao WSN` [#f2]_,
as regras sintácticas a aplicar na escrita de programas em linguagem *assembly* do P16.

.. code-block:: console
   :caption: Regras sintáticas da linguagem *assembly*
   :name: sintaxe

   program = statement { statement }.

   statement =
     [label] [instruction | direcive] “EOL” .

   directive =
     ( “.section” identifier )
     | “.text”
     | ".rodata"
     | “.data”
     | ".bss"
     | ".stack"
     | “.align” [ expression ]
     | “.equ” identifier “,” expression )
     | ( “.byte” | “.word” ) expression { “,” expression }
     | “.space” expression [ “,” expression ] )
     | ( “.ascii” | “.asciz” ) string { “,” string } .

   instruction =
     “ldr” reg0-15 “,” ( “[” (“pc” | “r15”) “,” expression “]” ) | identifier

     | ( ( “ldr” | “str” ) [“b”] reg0-15 “,”
        ( “[“ reg0-7 [“,” (reg0-15 | expression)] “]” )
     | ( "mov" | “movt” ) reg0-15, (reg0-15 | expression)
     | ( “push” | “pop” ) [“{“] reg0-15
     | ( “add” | “sub” ) reg0-15, reg0-7, (reg0-15 | expression)
     | ( “adc” | “sbc” ) reg0-15, reg0-7,  reg0-15
     | “cmp” reg0-7, reg0-15
     | ( “and” | “orr” | “eor” ) reg0-15, reg0-7, reg0-15
     | ( “mvn” | “not”) reg0-15, reg0-15
     | ( “lsl” | “lsr” | “asr” | “ror” )	reg0-15, reg0-7, expression
     | “rrx” reg0-15, reg0-7
     | “msr” psw “," reg0-15
     | “mrs” reg0-15 “,” psw
     | ( “bzs” | “beq” | “bzc” | “bne” | “bcs” | “blo” | “bcc” | “bhs”
     | “blt” | “bge” | “bl” | “b” ) expression
     | “movs pc, lr” .

   reg0-7 = “r0” | “r1” | “r2” | “r3” | “r4” | “r5” | “r6” | “r7” .
      | “R0” | “R1” | “R2” | “R3” | “R4” | “R5” | “R6” | “R7” .

   reg0-15 = reg0-7
     | “r8” | “r9” | “r10” | “r11” | “r12” | “r13” | “r14” | “r15”
     | “R8” | “R9” | “R10” | “R11” | “R12” | “R13” | “R14” | “R15”
     | “sp” | “lr” | “pc” | “SP” | “LR” | “PC” .

   psw = “cpsw” | “spsw” | “CPSW” | “SPSW”.

   expression = logical_or_expression
     | logical_or_expression “?” expression “:” expression .

   logical_or_expression = logical_and_expression
     | logical_or_expression “||” logical_and_expression .

   logical_and_expression = inclusive_or_expression
     | logical_and_expression “&&” inclusive_or_expression .

   inclusive_or_expression = exclusive_or_expression
     | inclusive_or_expression “|” exclusive_or_expression .

   exclusive_or_expression = and_expression
     | exclusive_or_expression “^” and_expression .

   and_expression = equality_expression
     | and_expression “&” equality_expression .

   equality_expression = relational_expression
     | equality_expression “==” relational_expression
     | equality_expression “!=” relational_expression .

   relational_expression = shift_expression
     | relational_expression “<” shift_expression
     | relational_expression “>” shift_expression
     | relational_expression “<=” shift_expression
     | relational_expression “>=” shift_expression .

   shift_expression = additive_expression
     | shift_expression “<<” additive_expression
     | shift_expression “>>” additive_expression .

   additive_expression = multiplicative_expression
     | additive_expression “+” multiplicative_expression
     | additive_expression “-” multiplicative_expression .

   multiplicative_expression = unary_expression
     | multiplicative_expression “*” unary_expression
     | multiplicative_expression “/” unary_expression
     | multiplicative_expression “%” unary_expression .

   unary_expression = primary_expression
     | “+” primary_expression
     | “-” primary_expression
     | “!” primary_expression
     | “~” primary_expression .

   primary_expression = literal | identifier | “(” expression “)” .

   identifier = ("." | alphabet | “_”) { "." | alphabet | number | “_” }.

   label =  identifier “:” .

   literal = decimal | hexadecimal | octal | binary | “’” character “’” .

   decimal = “0” | ((“1” | ... | “9”) { decimal_digit } ) .

   hexadecimal = “0” (“x” | “X”) hexadecimal_digit { hexadecimal_digit } .

   octal = “0” (“1” | ... | “7”) { octal_digit } .

   binary = “0” (“b” | “B”) (“0” | “1”) { “0” | “1” } .

   octal_digit = “0” | “1” | ... | “6” | “7” .

   decimal_digit = “0” | “1” | ... | “8” | “9” .

   hexadecimal_digit = decimal_digit | “a” | ... | “f” | “A” | ... | “F” .

   alphabet = “a” | ... | “z” | “A” | ... | “Z” .

   character = alphabet | decimal_digit 
     | “[” | “]” | “{” | “}” | “(” | “)” | “<” | “>”
     | “=” | “|” | “&” | “%” | “$” | “#” | “/” | “?” | “!” | “_” | “*”
     | “\b” | “\t” | “\n” | “\f” | “\r” | “\\” | “\"” | “\'”
     | ( “\” ( decimal | hexadecimal | octal | binary ) ) .
   
   string = “\”” character { character } “\”” .

   “EOL” = control character for end of line

Limitações sintáticas
---------------------

* Não é possível definir símbolos iguais a mnemónicas de instruções.
  Por exemplo, não pode existir um símbolo \"b\"
  porque coincide com a mnemónica da instrução *branch*.

* Se ocorrer omissão do último elemento sintático, a visualização do erro é assinalada corretamente, mas é dada indicação da linha seguinte.

.. rubric:: Footnotes

.. [#f1] https://pt.wikipedia.org/wiki/Case-sensitive

.. [#f2] https://en.wikipedia.org/wiki/Wirth_syntax_notation

