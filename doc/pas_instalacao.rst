Instalação
==========

Instalação do executável
------------------------

Window
^^^^^^
   Descarregar `daqui <https://www.dropbox.com/s/pr1nl6yg2hbxkzd/pas.exe?dl=0>`_.

   (Falta instruções de instalação)

Linux
^^^^^
   Descarregar `daqui <https://www.dropbox.com/s/dtw6l7li4cug9cc/pas?dl=0>`_.

   É necessário redefinir o atributo de executável:

   ``$ chmod 755 pas``

   Copiar para a diretoria de instalação. Por exemplo:

   ``$ cp pas /usr/local/bin``

   Acrescentar o caminho ``/usr/local/bin`` na variável de ambiente ``PATH``.

   ``$ export $PATH:/usr/local/bin``

Instalação a partir da fonte
----------------------------

   Descarregar a fonte a partir do github.

   ``$ git clone https://www.github.com/econde/p16_assembler``

   ``$ cd p16_assembler``


Window
^^^^^^

   Devem estar disponíveis as ferramentas *cygwin* .

   ``$ sudo apt install g++-mingw-w64``

   ``$ make WINDOWS=1``

Linux
^^^^^

   ``$ make``
