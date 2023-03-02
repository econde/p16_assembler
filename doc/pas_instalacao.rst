Instalação
==========

Instalação do executável
------------------------

Em Window
^^^^^^^^^
   Descarregar `daqui <https://www.dropbox.com/s/38ntn2fucc5dgww/p16as.exe?dl=0>`_.

   Copiar para a diretoria de instalação. Por exemplo:

   ``C:> copy p16as.exe C:\Users\<utilizador>\AppData\Roaming``

   Acrescentar o caminho na variável de ambiente Path.

Em Linux
^^^^^^^^
   Descarregar `daqui <https://www.dropbox.com/s/1yrdlmad3i8s2zt/p16as?dl=0>`_.

   É necessário redefinir o atributo de executável:

   ``$ chmod 755 p16as``

   Copiar para a diretoria de instalação. Por exemplo:

   ``$ cp p16as /usr/local/bin``

   Acrescentar o caminho ``/usr/local/bin`` na variável de ambiente ``PATH``.

   ``$ export $PATH:/usr/local/bin``

Geração do executável
---------------------

   Descarregar a fonte a partir do github.

   ``$ git clone https://www.github.com/econde/p16_assembler``

   ``$ cd p16_assembler``


Para Window
^^^^^^^^^^^

   Para gerar para Windows devem estar disponíveis as ferramentas *cygwin*.

   ``$ sudo apt install g++-mingw-w64``

   ``$ make WINDOWS=1``

Para Linux
^^^^^^^^^^

   ``$ make``
