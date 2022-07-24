.intel_syntax noprefix  # ask gcc to use intel syntax
.global xtea_decrypt_block_V1   # tell gcc this is a global function
# void xtea_decrypt_block_V1(uint32_t v[2], uint32_t const key[4]);

.text
xtea_decrypt_block_V1:
mov r8, rdi # v pointer -> r8
mov r9, rsi # key pointer -> r9
mov esi, [rdi] # v[0] -> esi
mov ecx, [rdi + 4] # v[1] -> ecx
mov eax, 0x8DDE6E40 # sum = 0x9e3779b9
.Ldecryloop:
mov edx, eax
shr edx, 11 # sum >> 11
and edx, 3 # (sum >> 11) & 3
mov edi, DWORD PTR[r9 + rdx * 4] # key[(sum>>11) & 3]
add edi, eax # key[(sum>>11) & 3] + sum
mov r10d, esi 
shr r10d, 5 # v0 >> 5
mov edx, esi
sal edx, 4 # v0 << 4
xor edx, r10d # (v0 << 4)^(v0 >> 5)
add edx, esi # (v0 << 4)^(v0 >> 5) + v0
xor edx, edi # ((v0 << 4)^(v0 >> 5) + v0) ^ (key[(sum>>11) & 3] + sum)
sub ecx, edx # v1 -=....
sub eax, 0x9e3779b9
mov edx, eax 
and edx, 3 # sum & 3
mov edi, DWORD PTR [r9 + rdx * 4] # key[sum & 3]
add edi, eax # key[sum & 3] + sum
mov r10d, ecx
shr r10d, 5 # v1 >> 5
mov edx, ecx 
sal edx, 4 # v1 << 4
xor edx, r10d # (v1 << 4) ^ (v1 >> 5)
add edx, ecx # (v1 << 4) ^ (v1 >> 5) + v1
xor edx, edi # ((v1 << 4) ^ (v1 >> 5) + v1) ^ (key[sum & 3] + sum)
sub esi, edx # v0 -=....
test eax, eax
jne .Ldecryloop
mov DWORD PTR [r8], esi
mov DWORD PTR [r8 + 4], ecx
ret