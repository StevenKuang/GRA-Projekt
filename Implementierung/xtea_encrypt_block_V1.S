.intel_syntax noprefix  # ask gcc to use intel syntax
.global xtea_encrypt_block_V1   # tell gcc this is a global function
# void xtea_encrypt_block_V1(uint32_t v[2], uint32_t const key[4]);
.text
xtea_encrypt_block_V1:
mov r8, rdi # v pointer -> r8
mov r9, rsi # key pointer -> r9
mov esi, [rdi] # v[0] -> esi
mov ecx, [rdi + 4] # v[1] -> ecx
xor eax, eax # sum = 0
.Lencryloop:
# first part v1+=..
mov edx, eax
and edx, 3 # sum & 3
mov edi, DWORD PTR [r9 + rdx * 4] # key[sum & 3] -> edi
add edi, eax # sum + key[sum & 3]
mov r10d, ecx 
shr r10d, 5 # v1 >> 5
mov edx, ecx
sal edx, 4 # v1 << 4
xor edx, r10d # (v1 << 4) ^ (v1 >> 5)
add edx, ecx # ((v1 << 4) ^ (v1 >> 5)) + v1
xor edx, edi # (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3])
add esi, edx # v0 +=
add eax, 0x9E3779B9
# second part v0+=..
mov edx, eax 
shr edx, 11 # sum >> 11
and edx, 3 # (sum >> 1) & 3
mov edi, DWORD PTR [r9 + rdx * 4] # key[(sum >> 1) & 3]
add edi, eax # sum + key[(sum >> 1) & 3] 
mov r10d, esi
shr r10d, 5 # v0 >> 5
mov edx, esi
sal edx, 4 # v0 << 4
xor edx, r10d # (v0 << 4) ^ (v0 >> 5)
add edx, esi # (v0 << 4) ^ (v0 >> 5) + v0
xor edx, edi # ((v0 << 4) ^ (v0 >> 5) + v0) ^(sum + key[(sum >> 1) & 3])
add ecx, edx # v1+=..
cmp eax, 0x8DDE6E40 # low 32bit 0x9e3779b9 * 0x40
jne .Lencryloop
mov DWORD PTR [r8], esi
mov DWORD PTR [r8 + 4], ecx 
ret