.intel_syntax noprefix  # ask gcc to use intel syntax
.global decryptOptimized   # tell gcc this is a global function
# void encryptOptimized(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]);

.text
decryptOptimized:
    # TODO
    ret