static const unsigned char rsa_pub_key[] = R"~~~(-----BEGIN PUBLIC KEY-----
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAnDIr2mvSi837W7qaQ/Io
4IM+wDXr4DsDpXI5Aa/6RJhTUuIUShQWxe4pLIT28l3VXvEoOXTkZYlXhSIfneae
fKXTB1L8BF7ZpVxGXs/ugOP9gAJFvXztq8caZCZJ8UNx8qCxlTW3YurjAQFmSZpe
O+5L/CtTfLjq6MNeb77n7mVMp2S0Aqksyt0/tqaw+3lTFiKlupdghykphZ2GWsdq
5z8627UaXMRmc5AGDeSO+nfECvmJjS9YlM7zIJL32tTtFo0AvDecxdazt+rjEbxC
4xsa+vykp9eTQrGySrD9IIvE/wECYNUAnSDqVqLl+yQUx9gtd3/ufldiTCA+yP9m
ADrzrtryOiqQfd6in+LteoGLICUkmfEwAUAIwXGipMQnybvPRygoRp4AyfejvJqW
cJFHRzAWEG1w6BcDpxu4PRRK5RgDkH7YvYjrhk7KnqMRqko4gmSTvfkboJFr3pna
J2ZBM2cuORIS+63yNSD3JtzU8e8FeRy97ayEw99IaPxBz5fpAmfR7oF1rYFuEtZq
ubkLJB8EFBIO8u9hgI0ZhMzFQNcbRV9gpKQ01Uui3NHnr3XV4A2cnxw755u/ABmy
VgHDYK6a15KTpYjDgzJwjgDSoDRigRPV9jgkXvagFqS8PGLmSDR4GTwy6IxZGt3D
4bXGfMA4siMGqEVIRj3Z3l8CAwEAAQ==
-----END PUBLIC KEY-----
)~~~";

/*
- This file is for conveniently storing rsa_public_key to use/embed in the fimrware.
- You should only include this file once and only-once in the main.cpp source file.
- ... Then use the rsa_pub_key as a global char pointer to the raw_rsa_pub_key_c_str.
- Usage:
    + Gen the rsa_key.pub as raw format from openssl, for example. 
    + Copy and paste the raw content in rsa_key.pub into the space between a "Raw string literal" as seen above. 
    + `R"~~~(<your_rsa_raw_text_here_without_any_extra_space_or_newline)~~~"` 
*/