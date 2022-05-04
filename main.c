#include <openssl/evp.h>
// #include <openssl/applink.c>
#include "common.h"
#include "account.h"
#include "digest.h"
#include "sign.h"

#define TEST_SIGNATURE_MSG 0
#define TEST_SIGNATURE_FILE 0
#define TEST_HASH_FILE 0
#define TEST_HEX_2_BIN 0
#define TEST_MERKLE_TREE 0
#define TEST_MERKLE_TREE_2 0
#define TEST_MERKLE_TREE_3 0
#define TEST_MERKLE_TREE_4 1

int main(void)
{
  printf("Hello blockchain !\n");

#if TEST_MERKLE_TREE_4 // digest_hex_2_bin_bulk : cr�er un array de hash binaires concaten�s

  while (1)
  {
    uint32_t endianess = 0xA0B70708;
    uint32_t* p = &endianess;

    uint8_t* const const hashes[4] = {
        "8c14f0db3df150123e6f3dbbf30f8b955a8249b62ac1d1ff16284aefa3d06d87",
        "fff2525b8931402dd09222c50775608f75787bd2b87e56995a7bdd30f79702c4",
        "6359f0868171b1d194cbee1af2f16ea598ae8fad666d9b012c8ed2b79a236ec4",
        "e9a66845e05d5abc0ad04ec80f774a7e585c6e8db975962d069a522137b80c1d"
        //"51a3dd31a49acb157d010f08e5c4774721d6dd39217866f2ed42d209b66a6ff6",
        //"50ba87bdd484f07c8c55f76a22982f987c0465fdc345381b4634a70dc0ea0b38",
        //"96b8787b1e3abed802cff132c891c2e511edd200b08baa9eb7d8942d7c5423c6",
        //"65e5a4862b807c83b588e0f4122d4ca2d46691d17a1ec1ebce4485dccc3380d4",
        //"1ee9441ddde02f8ffb910613cd509adbc21282c6e34728599f3ae75e972fb815",
        //"ec950fc02f71fc06ed71afa4d2c49fcba04777f353a001b0bba9924c63cfe712",
        //"5d874040a77de7182f7a68bf47c02898f519cb3b58092b79fa2cff614a0f4d50",
        //"0a1c958af3e30ad07f659f44f708f8648452d1427463637b9039e5b721699615",
        //"d94d24d2dcaac111f5f638983122b0e55a91aeb999e0e4d58e0952fa346a1711",
        //"c4709bc9f860e5dff01b5fc7b53fb9deecc622214aba710d495bccc7f860af4a",
        //"d4ed5f5e4334c0a4ccce6f706f3c9139ac0f6d2af3343ad3fae5a02fee8df542",
        //"b5aed07505677c8b1c6703742f4558e993d7984dc03d2121d3712d81ee067351",
        //"f9a14bf211c857f61ff9a1de95fc902faebff67c5d4898da8f48c9d306f1f80f"
    };

    uint8_t* bin_hashes = NULL;
    bin_hashes = digest_hex_2_bin_bulk_to_lendian(hashes, 4);

    uint8_t merkle_root[SHA256_DIGEST_LENGTH] = { 0 };
    digest_merkle_root(merkle_root, 4, bin_hashes);

    uint8_t hex_root[HEX_HASH_NT_SZ] = { 0 };

    digest_bin_2_hex(hex_root, HEX_HASH_NT_SZ, merkle_root, SHA256_DIGEST_LENGTH);

    printf("Merkle root is : %s\n", hex_root);

    free(bin_hashes);
    bin_hashes = NULL;
    break;
  }

  // no leaks

#endif

#if TEST_MERKLE_TREE_3 // test digest_hash_bin_pair_leaves

  while (1)
  {
    uint8_t* const path_left_leave = concat(DEBUG_PATH, "left_leave.bin");
    uint8_t* const path_right_leave = concat(DEBUG_PATH, "right_leave.bin");
    uint8_t* const path_pair_leave = concat(DEBUG_PATH, "pair_leave.bin");

    LeavesPair leaves_pair = { 0 };
    uint8_t concat_leaves_bin[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t concat_leave_hex[HEX_HASH_NT_SZ] = { 0 };

    uint8_t* const hashes[2] = {
        "51a3dd31a49acb157d010f08e5c4774721d6dd39217866f2ed42d209b66a6ff6",
        "50ba87bdd484f07c8c55f76a22982f987c0465fdc345381b4634a70dc0ea0b38",
    };

    digest_hex_2_bin(leaves_pair.left, SHA256_DIGEST_LENGTH, hashes[0], HEX_HASH_NT_SZ);
    digest_hex_2_bin(leaves_pair.right, SHA256_DIGEST_LENGTH, hashes[1], HEX_HASH_NT_SZ);

    // digest_pair_bin_leaves(&leaves_pair, leaves_pair.left, leaves_pair.right);

    digest_hash_bin_pair_leaves(concat_leaves_bin, &leaves_pair);

    digest_bin_2_hex(concat_leave_hex, HEX_HASH_NT_SZ, concat_leaves_bin, SHA256_DIGEST_LENGTH);

    printf("hash leave 1 : %s\n", hashes[0]);
    printf("hash leave 2 : %s\n", hashes[1]);
    printf("hash concatenated pair : %s\n", concat_leave_hex);

    digest_wb32_file(path_left_leave, SHA256_DIGEST_LENGTH, leaves_pair.left);
    digest_wb32_file(path_right_leave, SHA256_DIGEST_LENGTH, leaves_pair.right);
    digest_wb64_file(path_pair_leave, SHA256_DIGEST_LENGTH, &leaves_pair);

    // ATTENTION
    // avec windows je hashe le produit du hash final sha256(32 bytes) et non les deux hash concat sha256(l1, l2);
    // il faut donc que je wb les deux buffers dans un fichier, et ensuite que j'y applique le hash windows
    // le fichier ir_leaves doit faire 64bytes

    uint8_t hashed_file[SHA256_DIGEST_LENGTH] = { 0 };

    digest_hash_file(hashed_file, path_pair_leave);

    uint8_t hashed_file_hex[HEX_HASH_NT_SZ] = { 0 };

    digest_bin_2_hex(hashed_file_hex, HEX_HASH_NT_SZ, hashed_file, SHA256_DIGEST_LENGTH);

    assert(strcmp(hashed_file_hex, concat_leave_hex) == 0);

    printf("hash file : %s\n", hashed_file_hex);

    free(path_left_leave);
    free(path_right_leave);
    free(path_pair_leave);

    // break;
  }

#endif

#if TEST_MERKLE_TREE_2

  // uint8_t leaf_left[SHA256_DIGEST_LENGTH] = { 0 };
  // uint8_t leaf_right[SHA256_DIGEST_LENGTH] = { 0 };
  while (1)
  {
    LeavesPair leaves_pair = { 0 };
    // uint8_t two_leaves[SHA256_DIGEST_LENGTH * 2] = { 0 };
    uint8_t leave_one_hex[HEX_HASH_NT_SZ] = { 0 };
    uint8_t leave_two_hex[HEX_HASH_NT_SZ] = { 0 };

    uint8_t* const hashes[2] = {
        "51a3dd31a49acb157d010f08e5c4774721d6dd39217866f2ed42d209b66a6ff6",
        "50ba87bdd484f07c8c55f76a22982f987c0465fdc345381b4634a70dc0ea0b38",
    };

    digest_hex_2_bin(leaves_pair.left, SHA256_DIGEST_LENGTH, hashes[0], HEX_HASH_NT_SZ);
    digest_hex_2_bin(leaves_pair.right, SHA256_DIGEST_LENGTH, hashes[1], HEX_HASH_NT_SZ);

    digest_pair_bin_leaves(&leaves_pair, leaves_pair.left, leaves_pair.right);

    digest_bin_2_hex(leave_one_hex, HEX_HASH_NT_SZ, leaves_pair.left, SHA256_DIGEST_LENGTH);
    digest_bin_2_hex(leave_two_hex, HEX_HASH_NT_SZ, leaves_pair.right, SHA256_DIGEST_LENGTH);

    printf("hash two leaves : %s\n", leave_one_hex);
    printf("hash two leaves : %s\n", leave_two_hex);

    // no leak
  }

#endif
#if TEST_MERKLE_TREE
  size_t i;
  uint8_t tx_bin_temp[13 * SHA256_DIGEST_LENGTH] = { 0 };
  uint8_t mk_p_bin_final[SHA256_DIGEST_LENGTH] = { 0 };
  // uint8_t merkle_proof[13 * SHA256_DIGEST_LENGTH] = { 0 };

  // uint8_t* p = merkle_proof + 32;

  // uint8_t* const txid = "51a3dd31a49acb157d010f08e5c4774721d6dd39217866f2ed42d209b66a6ff6";

  // digest_hex_2_bin(mr, SHA256_DIGEST_LENGTH, txid, HEX_HASH_NT_SZ, true);

  // memcpy(merkle_proof, mr, SHA256_DIGEST_LENGTH);

  uint8_t* const hashes[13] = {
      "51a3dd31a49acb157d010f08e5c4774721d6dd39217866f2ed42d209b66a6ff6",
      "50ba87bdd484f07c8c55f76a22982f987c0465fdc345381b4634a70dc0ea0b38",
      "96b8787b1e3abed802cff132c891c2e511edd200b08baa9eb7d8942d7c5423c6",
      "65e5a4862b807c83b588e0f4122d4ca2d46691d17a1ec1ebce4485dccc3380d4",
      "1ee9441ddde02f8ffb910613cd509adbc21282c6e34728599f3ae75e972fb815",
      "ec950fc02f71fc06ed71afa4d2c49fcba04777f353a001b0bba9924c63cfe712",
      "5d874040a77de7182f7a68bf47c02898f519cb3b58092b79fa2cff614a0f4d50",
      "0a1c958af3e30ad07f659f44f708f8648452d1427463637b9039e5b721699615",
      "d94d24d2dcaac111f5f638983122b0e55a91aeb999e0e4d58e0952fa346a1711",
      "c4709bc9f860e5dff01b5fc7b53fb9deecc622214aba710d495bccc7f860af4a",
      "d4ed5f5e4334c0a4ccce6f706f3c9139ac0f6d2af3343ad3fae5a02fee8df542",
      "b5aed07505677c8b1c6703742f4558e993d7984dc03d2121d3712d81ee067351",
      "f9a14bf211c857f61ff9a1de95fc902faebff67c5d4898da8f48c9d306f1f80f" };

  for (i = 0; i < 13; i++)
  {
    digest_hex_2_bin(&(tx_bin_temp[i * SHA256_DIGEST_LENGTH]), SHA256_DIGEST_LENGTH, hashes[i], HEX_HASH_NT_SZ - 1);
    // memcpy((uint8_t*)(p), mr, SHA256_DIGEST_LENGTH);
    // p += 32;
  }

  uint8_t* const merkle_root = "17663ab10c2e13d92dccb4514b05b18815f5f38af1f21e06931c71d62b36d8af";

  // size_t test = strlen(merkle_proof);

  uint8_t mrhex[HEX_HASH_NT_SZ] = { 0 };

  digest_hash_merkle_proof(mk_p_bin_final, tx_bin_temp, 13 * SHA256_DIGEST_LENGTH);
  digest_bin_2_hex(mrhex, HEX_HASH_NT_SZ, mk_p_bin_final, SHA256_DIGEST_LENGTH);

  printf("Target merkle root :\n%s\nActual merkle root :\n%s\n", merkle_root, mrhex);

#endif

#if TEST_HEX_2_BIN
  while (1)
  {

    uint8_t* const txid = "51a3dd31a49acb157d010f08e5c4774721d6dd39217866f2ed42d209b66a6ff6";

    size_t len = 0;

    uint8_t hexhash[HEX_HASH_NT_SZ] = { 0 };
    uint8_t binhash[SHA256_DIGEST_LENGTH] = { 0 };

    digest_hex_2_bin(binhash, SHA256_DIGEST_LENGTH, txid, HEX_HASH_NT_SZ);

    digest_bin_2_hex(hexhash, HEX_HASH_NT_SZ, binhash, len);

    printf("%s\n", txid);

    // no leak
  }

#endif

#if 0
  while (0)
  {
    EVP_PKEY* key = account_keys_create();

    account_keys_write_PEM_file(key, DEBUG_PATH);

    EVP_PKEY_free(key);
  }

  EVP_PKEY* key = account_keys_create();


  const uint8_t* path = concat(DEBUG_PATH, "1646733517396.webm");


  uint8_t hashResult[SHA256_DIGEST_LENGTH] = { 0 };
  uint8_t hexHash[HEX_HASH_NT_LEN] = { 0 };
  //uint8_t signature[SHA256_DIGEST_LENGTH] = { 0 };

  digest_hash_file(hashResult, path);

  hash2Hex(hexHash, hashResult);

  printf("hash fichier : ");
  printf("%s\n", hexHash);


  digest_hash_str(hashResult, "Hello world !");
  hash2Hex(hexHash, hashResult);
  printf("hash string : %s\n", hexHash);

  Personne personne = { .prenom = "Antony", .nom = "Merle", .taille = 177 };

  printf("%s %s %zu cm\n", personne.prenom, personne.nom, personne.taille);


  digest_hash_file(hashResult, path);

  free(path);

  /* =================================================== */

  uint8_t* signature = signMsg2(key, hashResult);

  // TODO : implement verification fonction. fn verifier(message, signature, pk) -> bool

  //memset(hashResult, '\0', SHA256_DIGEST_LENGTH);

  //hashData(&personne, hashResult);
  //uint8_t* hashStruct = hash2Hex(hexHash, hashResult);		// TODO : hashe le pointeur mais pas le contenu. Faire une fonction par type.


  //printf("hash struct : %s\n", hashStruct);

  EVP_PKEY_free(key);

  free(signature);

#endif
  /* =================================================== */

#if 0
  const uint8_t* pathKey = concat(DEBUG_PATH, "private.pem");

  EVP_PKEY* keyfromfile = EVP_PKEY_new();

  account_keys_load_PEM_file(&keyfromfile, pathKey);

  BIO* bio = NULL;
  bio = BIO_new_fp(stdout, BIO_NOCLOSE);

  account_keys_print_one_PEM(keyfromfile, PKEY);
  puts("\n");
  account_keys_print_one_PEM(keyfromfile, SKEY);
  puts("\n");




  EVP_PKEY_free(keyfromfile);
  BIO_free(bio);
  free(pathKey);
#endif
  // no leak

#if TEST_SIGNATURE_MSG

  while (1)
  {
    // const uint8_t* path = concat(DEBUG_PATH, "1646733517396.webm");

    uint8_t hashResult[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t hexHash[HEX_HASH_NT_LEN] = { 0 };

    // digest_hash_file(hashResult, path);
    digest_hash_str(hashResult, "Hello world !");

    hash2Hex(hexHash, hashResult);

    printf("hash string : ");
    printf("%s\n", hexHash);

    const uint8_t* pathKey = concat(DEBUG_PATH, "private.pem");

    EVP_PKEY* keyfromfile = EVP_PKEY_new();

    account_keys_load_PEM_file(&keyfromfile, pathKey);

    BIO* bio = NULL;
    bio = BIO_new_fp(stdout, BIO_NOCLOSE);

    account_keys_print_one_PEM(keyfromfile, PKEY);
    puts("\n");
    account_keys_print_one_PEM(keyfromfile, SKEY);
    puts("\n");

    size_t slen = 0;
    uint8_t* signature = sign_msg(keyfromfile, "Hello world !");

    printf("\nmain.c Signature is: ");
    int i;
    for (i = 0; i < 256; i++)
      printf("%.2X ", signature[i]);
    printf("\n");

    bool legit = false;
    // memset(hashResult, 0, SHA256_DIGEST_LENGTH);

    legit = sign_verify_str_msg(keyfromfile, signature, "Hello world !");

    legit ? printf("La signature est valide\n") : printf("La signature est invalide\n");

    EVP_PKEY_free(keyfromfile);
    BIO_free(bio);

    free(pathKey);
    // free(path);
    free(signature);
    break;

    // no leak
  }

#endif

#if TEST_SIGNATURE_FILE

  while (1)
  {
    uint8_t* filePath = concat(DEBUG_PATH, "1646733517396.webm");

    uint8_t* pathKey = concat(DEBUG_PATH, "private.pem");

    EVP_PKEY* keyfromfile = account_keys_create();

    // BIO* bio = NULL;
    // bio = BIO_new_fp(stdout, BIO_NOCLOSE);

    // TODO : fn printKeys

    account_keys_print_pair_PEM(keyfromfile);

    // size_t slen = 0;
    uint8_t signature[SIG_BIN_SZ] = { 0 };
    sign_msg(signature, keyfromfile, "Hello world !");

    uint8_t hexBuffer[SIG_HEX_NT_SZ] = { 0 };
    digest_bin_2_hex(hexBuffer, SIG_HEX_NT_SZ, signature, SIG_BIN_SZ);

    digest_hex_pretty_print(hexBuffer);

    bool legit = false;

    legit = sign_verify_str_msg(keyfromfile, signature, "Hello world !");

    legit ? printf("La signature est valide\n") : printf("La signature est invalide\n");

    printf("\nsign file\n");

    memset(signature, 0, SIG_BIN_SZ);
    memset(hexBuffer, 0, SIG_HEX_NT_SZ);

    sign_file(signature, keyfromfile, filePath);
    digest_bin_2_hex(hexBuffer, SIG_HEX_NT_SZ, signature, SIG_BIN_SZ);
    digest_hex_pretty_print(hexBuffer);

    legit = false;

    uint8_t* filePath2 = concat(DEBUG_PATH, "test.py");

    legit = sign_verify_file_sig(keyfromfile, signature, filePath);

    legit ? printf("La signature est valide\n") : printf("La signature est invalide\n");

    EVP_PKEY_free(keyfromfile);

    free(pathKey);
    free(filePath);
    free(filePath2);

    break;
    // no leak
  }

#endif

#if TEST_HASH_FILE

  while (1)
  {
    uint8_t* path = concat(DEBUG_PATH, "1646733517396.webm");

    uint8_t bin_hash[SHA256_DIGEST_LENGTH] = { 0 };
    uint8_t hex_hash[HEX_HASH_NT_SZ] = { 0 };

    digest_hash_file(bin_hash, path);
    digest_bin_2_hex(hex_hash, HEX_HASH_NT_SZ, bin_hash, SHA256_DIGEST_LENGTH);

    printf("Le hash de %s est :\n%s\n", path, hex_hash);

    free(path);
    break;
  }

  // test ok, no leak

#endif

  return EXIT_SUCCESS;
}