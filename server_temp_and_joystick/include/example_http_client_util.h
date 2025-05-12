/**
 * Copyright (c) 2024 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef EXAMPLE_HTTP_CLIENT_UTIL_H
#define EXAMPLE_HTTP_CLIENT_UTIL_H

#include "lwip/apps/http_client.h"

/*! \brief Parâmetros usados para fazer requisição HTTP
 *  \ingroup pico_lwip
 */
typedef struct EXAMPLE_HTTP_REQUEST
{
    /*!
     * Nome do host, ex: www.raspberrypi.com
     */
    const char *hostname;
    /*!
     * URL para requisição, ex: /favicon.ico
     */
    const char *url;
    /*!
     * Função de callback para cabeçalhos, pode ser nula
     * @see httpc_headers_done_fn
     */
    httpc_headers_done_fn headers_fn;
    /*!
     * Função de callback para resultados do servidor, pode ser nula
     * @see altcp_recv_fn
     */
    altcp_recv_fn recv_fn;
    /*!
     * Função de callback para resultados finais da requisição, pode ser nula
     * @see httpc_result_fn
     */
    httpc_result_fn result_fn;
    /*!
     * Argumento para passar às funções de callback
     */
    void *callback_arg;
    /*!
     * Porta a ser usada. Uma porta padrão será escolhida se for zero
     */
    uint16_t port;
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    /*!
     * Configuração TLS, pode ser nula ou configurada corretamente.
     * Ex: altcp_tls_create_config_client(NULL, 0) usaria HTTPS sem certificado
     */
    struct altcp_tls_config *tls_config;
    /*!
     * Alocador TLS, usado internamente para configurar SNI (Server Name Indication)
     */
    altcp_allocator_t tls_allocator;
#endif
    /*!
     * Configurações do cliente HTTP LwIP
     */
    httpc_connection_t settings;
    /*!
     * Flag que indica quando a requisição está completa
     */
    int complete;
    /*!
     * Resultado geral da requisição HTTP, válido apenas quando 'complete' está ativo
     */
    httpc_result_t result;

} EXAMPLE_HTTP_REQUEST_T;

struct async_context;

/*! \brief Executa uma requisição HTTP de forma assíncrona
 *  \ingroup pico_lwip
 *
 * Executa a requisição HTTP de forma assíncrona
 *
 * @param context contexto assíncrono
 * @param req parâmetros da requisição HTTP. Deve ser inicializado com zero pelo menos, com hostname e url definidos com valores válidos
 * @return Se zero for retornado, a requisição foi feita e estará completa quando \em req->complete for verdadeiro ou quando o callback de resultado for chamado.
 *  Um valor diferente de zero indica erro.
 *
 * @see async_context
 */
int http_client_request_async(struct async_context *context, EXAMPLE_HTTP_REQUEST_T *req);

/*! \brief Executa uma requisição HTTP de forma síncrona
 *  \ingroup pico_lwip
 *
 * Executa a requisição HTTP de forma síncrona
 *
 * @param context contexto assíncrono
 * @param req parâmetros da requisição HTTP. Deve ser inicializado com zero pelo menos, com hostname e url definidos com valores válidos
 * @param result Retorna o resultado geral da requisição HTTP quando completa. Zero indica sucesso.
 */
int http_client_request_sync(struct async_context *context, EXAMPLE_HTTP_REQUEST_T *req);

/*! \brief Um callback de cabeçalho HTTP que pode ser passado para \em http_client_init ou \em http_client_init_secure
 *  \ingroup pico_http_client
 *
 * Implementação de callback de cabeçalho HTTP que apenas imprime os cabeçalhos na saída padrão
 *
 * @param arg argumento especificado na inicialização
 * @param hdr buffer(es) pbuf de cabeçalho (pode conter dados também)
 * @param hdr_len tamanho dos cabeçalhos em 'hdr'
 * @param content_len tamanho do conteúdo recebido nos cabeçalhos (-1 se não recebido)
 * @return se retornar != zero, a conexão é abortada
 */
err_t http_client_header_print_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len);

/*! \brief Um callback de recebimento HTTP que pode ser passado para http_client_init ou http_client_init_secure
 *  \ingroup pico_http_client
 *
 * Implementação de callback de recebimento HTTP que apenas imprime o corpo da requisição na saída padrão
 *
 * @param arg argumento especificado na inicialização
 * @param conn conexão do cliente HTTP
 * @param p buffer(es) pbuf do corpo
 * @param err Código de erro em caso de falha
 * @return se retornar != zero, a conexão é abortada
 */
err_t http_client_receive_print_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err);

#endif