#pragma once
#include <Windows.h>

typedef struct _SIGNER_FILE_INFO {
    DWORD   cbSize;
    LPCWSTR pwszFileName;
    HANDLE  hFile;
} SIGNER_FILE_INFO, * PSIGNER_FILE_INFO;
typedef struct _SIGNER_BLOB_INFO {
    DWORD   cbSize;
    GUID* pGuidSubject;
    DWORD   cbBlob;
    BYTE* pbBlob;
    LPCWSTR pwszDisplayName;
} SIGNER_BLOB_INFO, * PSIGNER_BLOB_INFO;
typedef struct _SIGNER_CONTEXT {
    DWORD cbSize;
    DWORD cbBlob;
    BYTE* pbBlob;
} SIGNER_CONTEXT, * PSIGNER_CONTEXT;

typedef struct _SIGNER_CERT_STORE_INFO {
    DWORD          cbSize;
    PCCERT_CONTEXT pSigningCert;
    DWORD          dwCertPolicy;
    HCERTSTORE     hCertStore;
} SIGNER_CERT_STORE_INFO, * PSIGNER_CERT_STORE_INFO;
typedef struct _SIGNER_SPC_CHAIN_INFO {
    DWORD      cbSize;
    LPCWSTR    pwszSpcFile;
    DWORD      dwCertPolicy;
    HCERTSTORE hCertStore;
} SIGNER_SPC_CHAIN_INFO, * PSIGNER_SPC_CHAIN_INFO;

typedef struct _SIGNER_ATTR_AUTHCODE {
    DWORD   cbSize;
    BOOL    fCommercial;
    BOOL    fIndividual;
    LPCWSTR pwszName;
    LPCWSTR pwszInfo;
} SIGNER_ATTR_AUTHCODE, * PSIGNER_ATTR_AUTHCODE;

typedef struct _SIGNER_SUBJECT_INFO {
    DWORD cbSize;
    DWORD* pdwIndex;
    DWORD dwSubjectChoice;
    union {
        SIGNER_FILE_INFO* pSignerFileInfo;
        SIGNER_BLOB_INFO* pSignerBlobInfo;
    };
} SIGNER_SUBJECT_INFO, * PSIGNER_SUBJECT_INFO;
typedef struct _SIGNER_CERT {
    DWORD cbSize;
    DWORD dwCertChoice;
    union {
        LPCWSTR                pwszSpcFile;
        SIGNER_CERT_STORE_INFO* pCertStoreInfo;
        SIGNER_SPC_CHAIN_INFO* pSpcChainInfo;
    };
    HWND  hwnd;
} SIGNER_CERT, * PSIGNER_CERT;
typedef struct _SIGNER_SIGNATURE_INFO {
    DWORD             cbSize;
    ALG_ID            algidHash;
    DWORD             dwAttrChoice;
    union {
        SIGNER_ATTR_AUTHCODE* pAttrAuthcode;
    };
    PCRYPT_ATTRIBUTES psAuthenticated;
    PCRYPT_ATTRIBUTES psUnauthenticated;
} SIGNER_SIGNATURE_INFO, * PSIGNER_SIGNATURE_INFO;
typedef struct _SIGNER_PROVIDER_INFO {
    DWORD   cbSize;
    LPCWSTR pwszProviderName;
    DWORD   dwProviderType;
    DWORD   dwKeySpec;
    DWORD   dwPvkChoice;
    union {
        LPWSTR pwszPvkFileName;
        LPWSTR pwszKeyContainer;
    };
} SIGNER_PROVIDER_INFO, * PSIGNER_PROVIDER_INFO;

HRESULT WINAPI SignerSign(
    _In_     SIGNER_SUBJECT_INFO* pSubjectInfo,
    _In_     SIGNER_CERT* pSignerCert,
    _In_     SIGNER_SIGNATURE_INFO* pSignatureInfo,
    _In_opt_ SIGNER_PROVIDER_INFO* pProviderInfo,
    _In_opt_ LPCWSTR               pwszHttpTimeStamp,
    _In_opt_ PCRYPT_ATTRIBUTES     psRequest,
    _In_opt_ LPVOID                pSipData
);
HRESULT WINAPI SignerTimeStamp(
    _In_     SIGNER_SUBJECT_INFO* pSubjectInfo,
    _In_     LPCWSTR             pwszHttpTimeStamp,
    _In_opt_ PCRYPT_ATTRIBUTES   psRequest,
    _In_opt_ LPVOID              pSipData
);
HRESULT WINAPI SignerTimeStampEx2(
    _Reserved_ DWORD               dwFlags,
    _In_       SIGNER_SUBJECT_INFO* pSubjectInfo,
    _In_       LPCWSTR             pwszHttpTimeStamp,
    _In_       ALG_ID              dwAlgId,
    _In_       PCRYPT_ATTRIBUTES   psRequest,
    _In_       LPVOID              pSipData,
    _Out_      SIGNER_CONTEXT** ppSignerContext
);
HRESULT WINAPI SignerTimeStampEx3(
    _In_       DWORD                  dwFlags,
    _In_       DWORD                  dwIndex,
    _In_       SIGNER_SUBJECT_INFO* pSubjectInfo,
    _In_       PCWSTR                 pwszHttpTimeStamp,
    _In_       PCWSTR                 pszAlgorithmOid,
    _In_opt_   PCRYPT_ATTRIBUTES      psRequest,
    _In_opt_   PVOID                  pSipData,
    _Out_      SIGNER_CONTEXT** ppSignerContext,
    _In_opt_   PCERT_STRONG_SIGN_PARA pCryptoPolicy,
    _Reserved_ PVOID                  pReserved
);
