#!/bin/bash

# Captura de erros
err_report() {
    echo -e "\nErro em '$0' na linha $1. Encerrando Script.\n"
    exit 1
}
trap 'err_report $LINENO' ERR

# Variaveis do script
ASYNCAPI_YAML_PATH="../../data/asyncapi/asyncapi.yaml"
OUTPUT_BUILD_DIR="./asyncapi-docs"
ASYNCAPI_DOCS_DIR="../../data/asyncapi"

echo "limpando conteudo do diretorio ${OUTPUT_BUILD_DIR}"
mkdir -p ${OUTPUT_BUILD_DIR}
rm -rfv ${OUTPUT_BUILD_DIR}
mkdir -p ${OUTPUT_BUILD_DIR}
echo "Limpeza finalizada."

# Chamar o @asyncapi/generator para gerar o template HTML do asyncapi.yaml
npx ag ${ASYNCAPI_YAML_PATH} @asyncapi/html-template -o ${OUTPUT_BUILD_DIR}

echo "limpando conteudo do diretorio ${ASYNCAPI_DOCS_DIR} e mantendo 'asyncapi.yaml'"
mkdir -p ${ASYNCAPI_DOCS_DIR}
find ${ASYNCAPI_DOCS_DIR} ! -name 'asyncapi.yaml' -type f -exec rm -fv {} +
find ${ASYNCAPI_DOCS_DIR} ! -name 'asyncapi.yaml' -type d -empty -exec rm -rfv {} +
mkdir -p ${ASYNCAPI_DOCS_DIR}
echo "Limpeza finalizada."

echo "Copiando documentação gerada do AsyncAPI"
cp -rpv ${OUTPUT_BUILD_DIR}/* ${ASYNCAPI_DOCS_DIR}
echo "Copia finalizada."
