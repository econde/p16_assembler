/*
Copyright 2018 Ezequiel Conde

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

namespace ast {

//  Tipo do valor associado a uma expressão:
//      - ABSOLUTE - valores absolutos calculáveis imediatamente
//      - LABEL - endereço relativo à secção a que esta label pertence
//      - UNDEFINED - apenas se aplica a simbolos; antes de ser avaliado ou símbolo indefinido
//      - INVALID - expressão que envolva operações invalidas sobre labels;
//          (label * CONST) (label + label) ...

enum Value_type {
	ABSOLUTE, LABEL, UNDEFINED, INVALID
};

extern const char *value_type_name[];

}

#endif
