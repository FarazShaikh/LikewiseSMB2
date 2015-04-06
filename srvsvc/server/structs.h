#ifndef __STRUCTS_H__
#define __STRUCTS_H__

typedef struct _ACCESS_LIST
{
    PSID        *ppSid;
    ACCESS_MASK AccessMask;
    ULONG       ulAccessType;

} ACCESS_LIST, *PACCESS_LIST;

typedef struct _SRVSVC_SRV_CONTEXT
{
    PACCESS_TOKEN pUserToken;
    PBYTE         pSessionKey;
    DWORD         dwSessionKeyLen;
    DWORD         dwAccessGranted;

} SRVSVC_SRV_CONTEXT, *PSRVSVC_SRV_CONTEXT;

typedef struct _NET_SHARE_INFO0
{
    PSTR pszName;
    PSTR pszPath;
    PSTR pszComment;
    PSTR pszSID;
} NET_SHARE_INFO0, *PNET_SHARE_INFO0;

typedef struct _NET_SHARE_INFO1
{
    PSTR pszName;
    PSTR pszPath;
    PSTR pszComment;
    PSTR pszSID;
} NET_SHARE_INFO1, *PNET_SHARE_INFO1;

typedef struct _NET_SHARE_INFO2
{
    PSTR pszName;
    PSTR pszPath;
    PSTR pszComment;
    PSTR pszSID;
} NET_SHARE_INFO2, *PNET_SHARE_INFO2;

typedef struct _NET_SHARE_INFO501
{
    PSTR pszName;
    PSTR pszPath;
    PSTR pszComment;
    PSTR pszSID;
} NET_SHARE_INFO501, *PNET_SHARE_INFO501;

typedef struct _NET_SHARE_INFO502
{
    PSTR pszName;
    PSTR pszPath;
    PSTR pszComment;
    PSTR pszSID;
} NET_SHARE_INFO502, *PNET_SHARE_INFO502;

typedef struct _NET_SHARE_INFO503
{
    PSTR pszName;
    PSTR pszPath;
    PSTR pszComment;
    PSTR pszSID;
} NET_SHARE_INFO503, *PNET_SHARE_INFO503;

typedef union __srvsvc_NetFileInfo
{
    FILE_INFO_2 info2;
    FILE_INFO_3 info3;

} FILE_INFO, *PFILE_INFO;

typedef struct _SRVSVC_CONFIG
{
    pthread_mutex_t mutex;

    /* path to lsarpc server socket for local procedure calls */
    CHAR szLsaLpcSocketPath[PATH_MAX + 1];

} SRVSVC_CONFIG, *PSRVSVC_CONFIG;

typedef struct _SRVSVC_RUNTIME_GLOBALS
{
    pthread_mutex_t mutex;

    SRVSVC_CONFIG   config;                      /* configuration settings */

    dcethread*      pRpcListenerThread;

    rpc_binding_vector_p_t pServerBinding;
    rpc_binding_vector_p_t pRegistryBinding;

    DWORD           dwStartAsDaemon;             /* Should start as daemon */

    LWIO_LOG_TARGET logTarget;                   /* where are we logging   */

    LWIO_LOG_LEVEL  maxAllowedLogLevel;          /* how much logging ?     */

    CHAR            szLogFilePath[PATH_MAX + 1]; /* log file path */

    BOOLEAN         bProcessShouldExit;          /* Process termination flag */

    DWORD           dwExitCode;                  /* Process Exit Code */

    PSECURITY_DESCRIPTOR_ABSOLUTE pServerSecDesc;
    PSECURITY_DESCRIPTOR_ABSOLUTE pSessionSecDesc; /* session info management */

    GENERIC_MAPPING genericMapping;

} SRVSVC_RUNTIME_GLOBALS, *PSRVSVC_RUNTIME_GLOBALS;

#endif /* __STRUCTS_H__ */
