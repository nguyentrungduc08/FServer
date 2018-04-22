/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../../header/ssl.h"

fssl::fssl(){
    OpenSSL_add_all_algorithms();
    SSL_library_init();
    SSL_load_error_strings();
    this->ctx = NULL;
    this->verify_client = true;
}

fssl::~fssl(){
    SSL_CTX_free(this->ctx);
}

void 
fssl::create_context(){
    this->ctx = SSL_CTX_new( TLSv1_2_server_method() );
    if ( this->ctx == NULL )
	{
            ERR_print_errors_fp(stderr);
            abort();
	}
}

void 
fssl::configure_context(std::string cerfile, std::string keyfile){
    
    printf("@log ssl: begin load certificates \n");
    
    if (SSL_CTX_load_verify_locations(this->ctx, cerfile.c_str(), keyfile.c_str()) != 1)
        ERR_print_errors_fp(stderr);

    if (SSL_CTX_set_default_verify_paths(this->ctx) != 1)
        ERR_print_errors_fp(stderr);
    
    if (SSL_CTX_use_certificate_file(this->ctx, cerfile.c_str(), SSL_FILETYPE_PEM) <= 0 ){
        ERR_print_errors_fp(stderr);
        printf("@log ssl: SSL_CTX_use_certificate_file \n");
        abort();
    }
    
    if (SSL_CTX_use_PrivateKey_file(this->ctx, keyfile.c_str(), SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stderr);
        printf("@log ssl: SSL_CTX_use_PrivateKey_file \n");
        abort();
    }
    
    if ( !SSL_CTX_check_private_key(this->ctx) ){
        fprintf(stderr, "Private key does not match the public certificate\n");
        printf("@log ssl: SSL_CTX_check_private_key \n");
        abort();
    }
    
    printf("@log ssl: LoadCertificates Compleate Successfully.....\n");
}

SSL_CTX* 
fssl::get_ctx(){
    return this->ctx;
}

void 
fssl::set_verify_client(bool is_verify){
    this->verify_client = is_verify;
}