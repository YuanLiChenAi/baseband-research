/*
	MBN Mobile Baseband firmware
*/

#ifndef MBN_H
#define MBN_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "../packed.h"

#define MBN_HEADER_MAX_SIZE 80
#define MBN_HEADER_MIN_SIZE 40
#define MBN_EIGHTY_BYTE_MAGIC 0x73D71034
#define MBN_ROOT_CERTIFICATE_SIZE 1024 * 6
#define MBN_MAX_ROOT_CERTIFICATES 4

typedef enum MBN_IMAGE_SEGMENTS {
  kMbnSegmentHeader = 1,
  kMbnSegmentCode,
  kMbnSegmentSignature,
  kMbnSegmentX509Chain
} mbn_image_segment_type_t;

PACKED(typedef struct {
	uint32_t  codeword;            /* Codeword/magic number defining flash type
								information. */
	uint32_t  magic;               /* Magic number */
	uint32_t  image_id;        /* image content */
	uint32_t  reserved1;          /* RESERVED */
	uint32_t  reserved2;          /* RESERVED */
	uint32_t  image_src;             /* Location of image in flash or e-hostdl in RAM. This is given in
								byte offset from beginning of flash/RAM.  */
	uint32_t image_dest_ptr;        /* Pointer to location to store RPM_SBL/e-hostdl in RAM.
								Also, entry point at which execution begins.
								*/
	uint32_t  image_size;      /* Size of RPM_SBL image in bytes */
	uint32_t  code_size;       /* Size of code region in RPM_SBL image in bytes */
	uint32_t signature_ptr;         /* Pointer to images attestation signature */
	uint32_t  signature_size;        /* Size of the attestation signature in
								bytes */
	uint32_t cert_chain_ptr;  /* Pointer to the chain of attestation
								certificates associated with the image. */
	uint32_t  cert_chain_size; /* Size of the attestation chain in bytes */

	uint32_t  oem_root_cert_sel;  /* Root certificate to use for authentication.
								Only used if SECURE_BOOT1 table_sel fuse is
								OEM_TABLE. 1 indicates the first root
								certificate in the chain should be used, etc */
	uint32_t  oem_num_root_certs; /* Number of root certificates in image.
								Only used if SECURE_BOOT1 table_sel fuse is
								OEM_TABLE. Denotes the number of certificates
								OEM has provisioned                          */

	uint32_t  reserved3;          /* RESERVED */
	uint32_t  reserved4;          /* RESERVED */
	uint32_t  reserved5;          /* RESERVED */
	uint32_t  reserved6;          /* RESERVED */
	uint32_t  reserved7;          /* RESERVED */
}) mbn_header80_t;

PACKED(typedef struct {
  uint32_t image_id;           /**< Identifies the type of image this header
								  represents (OEM SBL, AMSS, Apps boot loader,
								  etc.). */
  uint32_t header_vsn_num;     /**< Header version number. */
  uint32_t image_src;          /**< Offset from end of the Boot header where the
								  image starts. */
  uint32_t image_dest_ptr;     /**< Pointer to location to store image in RAM.
								  Also, ent676ry point at which image execution
								  begins. */
  uint32_t image_size;         /**< Size of complete image in bytes */
  uint32_t code_size;          /**< Size of code region of image in bytes */
  uint32_t signature_ptr;      /**< Pointer to images attestation signature */
  uint32_t signature_size;     /**< Size of the attestation signature in
								 bytes */
  uint32_t cert_chain_ptr;     /**< Pointer to the chain of attestation
								 certificates associated with the image. */
  uint32_t cert_chain_size;    /**< Size of the attestation chain in bytes */
}) mbn_header40_t;

typedef enum MbnImageId {
	kMbnImageNone           = 0x00,
	kMbnImageOemSbl    		= 0x01,
	kMbnImageAmss       	= 0x02,
	kMbnImageOcbl      		= 0x03,
	kMbnImageHash       	= 0x04,
	kMbnImageAppbl     		= 0x05,
	kMbnImageApps           = 0x06,
	kMbnImageHostDl         = 0x07,
	kMbnImageDsp1           = 0x08,
	kMbnImageFsbl           = 0x09,
	kMbnImageDbl            = 0x0A,
	kMbnImageOsbl           = 0x0B,
	kMbnImageDsp2           = 0x0C,
	kMbnImageEhostdl        = 0x0D,
	kMbnImageNandprg        = 0x0E,
	kMbnImageNorprg         = 0x0F,
	kMbnImageRamfs1         = 0x10,
	kMbnImageRamfs2         = 0x11,
	kMbnImageAdspQ5        	= 0x12,
	kMbnImageAppsKernel    	= 0x13,
	kMbnImageBackupRamfs   	= 0x14,
	kMbnImageSbl1           = 0x15,
	kMbnImageSbl2           = 0x16,
	kMbnImageRpm            = 0x17,
	kMbnImageSbl3           = 0x18,
	kMbnImageTz             = 0x19,
	kMbnImageSsdKeys       	= 0x1A,
	kMbnImageGen            = 0x1B,
	kMbnImageDsp3           = 0x1C,
	kMbnImageAcdb           = 0x1D,
	kMbnImageWdt            = 0x1E,
	kMbnImageMba            = 0x1F,
	kMbnImageLast 			= kMbnImageMba
} mbn_image_id_t;

typedef enum MbnParserFlag {
	kMbnParserFlagNone 			   = 0x00,
	kMbnParserFlagExtractCode 	   = 0x01,
	kMbnParserFlagExtractSignature = 0x02,
	kMbnParserFlagExtractX509 	   = 0x04,
	kMbnParserFlagAll		   	   = kMbnParserFlagExtractCode | 
	kMbnParserFlagExtractSignature | kMbnParserFlagExtractX509
} mbn_parser_flag_t;

typedef struct Mbn {
	const char *filePath;
	const char *imageName;
	int parserFlags;
	size_t fileSize;
	uint32_t imageId;
	uint32_t imageSrc;
	uint32_t imageDestPtr;
	uint32_t imageSize;
	uint32_t codeSize;
	uint32_t signaturePtr;
	uint32_t signatureSize;
	uint32_t certChainPtr;
	uint32_t certChainSize;
	uint32_t oemRootCertSelection;
	uint32_t oemNumberOfRootCerts;
	uint8_t* header;
	uint8_t* code;
	uint8_t* signature;
	uint8_t* x509;
} mbn_t;

size_t mbn_parser_readSegment(FILE* file, int offset, int size, size_t maxSize, uint8_t* dst);

#endif
