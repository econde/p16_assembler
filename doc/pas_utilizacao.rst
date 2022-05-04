Utilização
==========

A partir de um ficheiro de texto em linguagem *assembly*, produzido num editor de programas,
o PAS gera ficheiros com o código máquina específico do P16.

O PAS é um *assembler* de uma única passagem, que por razões didácticas, processa apenas um ficheiro
fonte e localiza o código gerado. A localização consiste em atribuir endereço absoluto ao programa,
tarefa que não é normalmente realizada pelo *assembler*.

Invocação
---------

O PAS é invocado na janela de comando segundo a seguinte sintaxe:

   .. code-block:: console

      pas [options] <program filename>

      options:
         --verbose
         -h, --help
         -v, --version
         -i, --input <source filename>
         -o, --output <base filename>
         -s, --section <section name>=<address>
         -f, --format hexintel | binary | logisim8 | logisim16
         -a, --addresses <from>-<to>
         -l, --interleave

Os erros e avisos são assinalados na janela de comando utilizada na invocação.

O ficheiro com o texto do programa em linguagem *assembly* -- ficheiro fonte --
assinalado como **<source filename>**, tem normalmente a extensão **'s'**.

No caso de não serem detetados erros, são gerados ficheiros com o nome de base (**<base filename>**) definido na
opção ``--output`` e com as extensões **lst** e outras dependendo do formato do ficheiro com o código binário.
Se esta opção for omitida, os ficheiros produzidos terão o mesmo nome base que o ficheiro fonte.

A opção ``--section`` permite definir o endereço de localização das secções.
Em caso de omissão desta opção as secções são localizadas a partir do endereço 0x0000,
pela ordem com que estão escritas no ficheiro fonte.

A opção ``--format`` permite definir o formato do ficheiro de saída com o código binário do programa.
A omissão desta opção é equivalente a ``--format hexintel``.

Os formatos possíveis são:

* ``--format hexintel``: ficheiro binário em formato Intel HEX;
* ``--format binary``: o conteúdo do ficheiro binário é uma imagem exata da memória;
* ``--format logisim8``: ficheiro binário em formato Logisim, organizado em palavras de 8 *bits*;
* ``--format logisim16``: ficheiro binário em formato Logisim, organizado em palavras de 16 *bits*.

A opção ``--addresses`` permite definir a gama de endereços cujo conteúdo é transcrito para o ficheiro binário de saída.
O conteúdo respeitante a endereços fora do intervalo especificado é omitido do ficheiro binário de saída.

A opção ``--interleave`` faz com que o código binário do programa seja repartido por dois ficheiros
 -- um com os *bytes* respeitantes aos endereços pares
 e outro com os *bytes* respeitantes aos endereços ímpares.
 Esta opção é ignorada se também for mencionada a opção ``--format logisim16``.

Localização das secções
-----------------------
A definição da localização em memória de cada secção pode ser explícita ou implícita.

A localização explícita é definida através da opção ``--section`` na invocação do PAS.

A localização implícita aplica-se às secções omissas na localização explicita,
localizando-as no endereço a seguir ao último endereço da secção
anterior, pela ordem em que estão escritas no ficheiro fonte do programa.

No caso de não ser explicitada a localização da primeira secção definida no programa,
esta é localizada no endereço 0x0000.

No caso da secção estar fragmentada, e aplicando-se a localização implícita,
a sua localização é definida pela posição do primeiro fragmento.

O endereço inicial de uma secção é localizado automaticamente num endereço par.

Formatos de saída
-----------------

O código binário do programa é guardado em ficheiro num de três formatos:
formato Intel HEX [#f1]_, formato binário e formato do simulador Logisim.

Formato binário
^^^^^^^^^^^^^^^

No formato binário o conteúdo do ficheiro produzido é a imagem exata do conteúdo
da memória do sistema ao executar o programa.
O primeiro byte do ficheiro corresponde ao endereço da primeira secção.
Ao que se segue o restante conteúdo da primeira secção e das secções seguintes.
Se existirem intervalos de espaço de endereçamento entre secções,
a esse espaço será preenchido com o valor zero.

Formato Logisim
^^^^^^^^^^^^^^^

O simulador Logisim simula dispositivos de memória RAM ou ROM cujo conteúdo pode
ser carregado a partir de ficheiro.
Na utilização do Logisim na simulação de sistemas baseados no P16
é necessário carregar neste dispositivos
o código binário dos programas, produzido pelo PAS.

O código binário é guardado em formato de texto como uma sequência
de valores numéricos representados em base hexadecimal.
As ocorrências de sucessivos valores iguais são representadas pela sequência N*X.
Sendo N o número de vezes que o valor ocorre e X o valor em si.

Exemplo de utilização
---------------------

Considere-se o seguinte programa como conteúdo do ficheiro ``multiply.s``.

**Código fonte:** :download:`multiply.s<code/multiply.s>`

.. literalinclude:: /code/multiply.s
   :language: asm
   :linenos:
   :caption: Ficheiro ``multiply.s``
   :name: ficheiro_multiply_s

O comando

   .. code-block:: console

      pas multiply.s -s .data=0x4000 -s .text=0x1000

traduz e localiza o programa.
As opções ``-s`` definem-se os endereços da secção ``.data`` em ``0x4000``
e da secção ``.text`` em ``0x1000``.
Por localização implicita, a secção ``.startup`` é localizada no endereço ``0x0000``
por ser a primeira que aparece no ficheiro,
a secção ``.bss`` é localizada no endereço ``0x4002`` porque vem a seguir a ``.data`` que tem uma dimensão 2
e a secção .stack é localizada no endereço ``0x4006`` por vir depois da ``.bss`` que tem dimensão 4.

Os erros são assinalados na própria janela de comandos. Foi introduzido um
erros de sintaxe apenas para exemplificar.

   .. code-block:: console

      multiply.s (51): 	ld	r0, addressof_m
      ----------------        ^^
      ERROR!	syntax error

Se o programa fonte não tiver erros, são produzidos dois ficheiros adicionais ``multiply.lst`` com
informação legível e ``multiply.hex`` com o código máquina.

A emissão de avisos não impede a geração do código binário como no seguinte caso:

   .. code-block:: console

      multiply.s (90): 	sub	r1, r1, 17
      ----------------                        ^^
      WARNING!	Expression's value = 17 (0x11) not encodable in 4 bit, truncate to 1 (0x1)

Faz parte de uma boa prática de programação corrigir o programa até suprimir a emissão de
mensagens de aviso.

Por uma questão de organização, é conveniente criar especificamente uma directoria para alojar os
ficheiros relacionados com um dado programa. No exemplo seguinte a directoria ``multiply`` aloja
todos os ficheiros relacionados com este programa: ``multiply.s``, ``multiply.lst`` e ``multiply.hex``.

   .. code-block::

      disciplinas
         |-- pe
         |-- ss
         |-- ac
            |-- documents
            |-- p16_code
               |-- divide
               |-- multiply
                  |-- multiply.s
                  |-- multiply.lst
                  |-- multiply.hex

Em seguida apresenta-se o conteúdo do ficheiro ``lst``. Este contém a tabela de secções,
a tabela de símbolos e a listagem das instruções.

Na tabela de secções listam-se as secções existentes, as gamas de endereços que ocupam e as
respectivas dimensões.

Na tabela de símbolos listam-se os símbolos definidos através de *label* ou através da directiva ``.equ``.
Por cada símbolo é dada a seguinte informação: identificador, tipo, valor associado e secção a que pertence.

Na listagem das instruções, são apresentados do lado esquerdo, na primeira coluna o número da linha
do ficheiro fonte, na segunda coluna os endereços da memória e na terceira coluna o respectivo conteúdo.

Na arquitectura do P16 as palavras formadas por dois *bytes* – designadas por *word* – ocupam duas
posições de memória consecutivas, o *byte* de menor peso toma a posição de endereço menor
e o *byte* de maior peso, a posição de endereço maior – *little ended format*.

O conteúdo da memória – código das instruções e valor das variáveis – é escrito na terceira coluna
como uma sequência de *bytes* pela ordem dos endereços que ocupam na memória. Por exemplo, na
linha 7 o código máquina da instrução ``mov r1, pc`` que ocupa os endereços 8 e 9, e tem o valor
``0xb781``, é representado pela sequência de *bytes* ``81`` ``B7``. Na linha 28,
a variável ``m``, do tipo ``.byte``, ocupa o endereço ``0x0048`` e o seu valor é ``20 (0x14)``.

.. literalinclude:: /code/multiply.lst
   :language: none
   :caption: Ficheiro ``multiply.lst``
   :name: ficheiro_multiply_lst

O ficheiro de extensão ``hex``, em formato Intel HEX, contém apenas o código binário das instruções e os
valores iniciais das variáveis, com a indicação dos endereços de memória onde serão carregados.

.. literalinclude:: /code/multiply.hex
   :language: none
   :caption: Ficheiro ``multiply.hex``
   :name: ficheiro_multiply_hex

O seu conteúdo é composto por tramas, formadas por uma marca inicial, a dimensão dos dados,
o endereço onde os dados serão carregados, o tipo da trama, os dados contidos na trama e um código
para detecção de eventual corrupção dos dados – soma de controlo.

.. figure:: figures/hexintel.png
   :name: hexintel
   :width: 374px
   :height: 174px
   :align: center

   Formato de uma trama Intel Hex.

A soma de contolo é calculada de modo que a adição, em módulo 0x100,
de todos os *bytes* que formam a trama, some zero.

A trama ``:00 0000 01 FF`` tem dimensão zero, invoca virtualmente o endereço zero
e é do tipo ``01`` -- *end of file*, o que conjuntamente suscita a soma de controlo ``FF``.
Serve para terminar o ficheiro.

.. rubric:: Footnotes

.. [#f1] https://en.wikipedia.org/wiki/Intel_HEX
