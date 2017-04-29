//
// fsfltview
//
// Sample: Using the filter manager for volume and filter instance enumeration.
//
// Note:   This code is need to run under administrator mode.
//
// Author: YAMASHITA Katsuhiro
//
#include <SDKDDKVer.h>

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <fltuser.h>  // for FltMgr

#if 0
#include <winternl.h> // for UNICODE_STRING
#else
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
#endif

#define _VOLUME_NAME_LENGTH 256

WORD g_wVersion = 0;

HRESULT
_FindFirst_VolumeInstance(
    PCWSTR pszVolumeName,
    INSTANCE_INFORMATION_CLASS dwInformationClass,
    LPVOID  *lpReturnedBuffer,
    LPHANDLE  lpVolumeInstanceFind
    )
{
    HRESULT hr;
    HANDLE hVolumeInstanceFind;
    DWORD BytesReturned;

    PVOID lpBuffer = NULL;
    DWORD dwBufferSize = 0;

    for(;;)
    {
        hr = FilterVolumeInstanceFindFirst(
                pszVolumeName,
                dwInformationClass,
                lpBuffer,
                dwBufferSize,
                &BytesReturned,
                &hVolumeInstanceFind
                );

        if( HRESULT_CODE(hr) == ERROR_INSUFFICIENT_BUFFER )
        {
            if( lpBuffer )
                free(lpBuffer);
            lpBuffer = malloc(BytesReturned);
            if( lpBuffer == NULL )
            {
                hr = E_OUTOFMEMORY;
                break;
            }
            dwBufferSize = BytesReturned;

            continue;
        }
        else
        {
            break;
        }
    }

    if( hr == S_OK && lpBuffer != NULL )
    {
        *lpReturnedBuffer = lpBuffer;
        *lpVolumeInstanceFind = hVolumeInstanceFind;
    }
    else
    {
        *lpReturnedBuffer = NULL;
        *lpVolumeInstanceFind = NULL;

        if( lpBuffer != NULL )
            free(lpBuffer);
    }

    return hr;
}

HRESULT
_FindNext_VolumeInstance(
    HANDLE  hVolumeInstanceFind,
    INSTANCE_INFORMATION_CLASS dwInformationClass,
    LPVOID  *lpReturnedBuffer
    )
{
    HRESULT hr;
    DWORD BytesReturned;

    PVOID lpBuffer = NULL;
    DWORD dwBufferSize = 0;

    for(;;)
    {
        hr = FilterVolumeInstanceFindNext(
                hVolumeInstanceFind,
                dwInformationClass,
                lpBuffer,
                dwBufferSize,
                &BytesReturned
                );

        if( HRESULT_CODE(hr) == ERROR_INSUFFICIENT_BUFFER )
        {
            if( lpBuffer )
                free(lpBuffer);
            lpBuffer = malloc(BytesReturned);
            if( lpBuffer == NULL )
            {
                hr = E_OUTOFMEMORY;
                break;
            }
            dwBufferSize = BytesReturned;

            continue;
        }
        else
        {
            break;
        }
    }

    if( hr == S_OK && lpBuffer != NULL )
    {
        *lpReturnedBuffer = lpBuffer;
    }
    else
    {
        *lpReturnedBuffer = NULL;

        if( lpBuffer != NULL )
            free(lpBuffer);
    }
    return hr;
}

void print_string(PCSTR Title,PVOID pBuffer,USHORT offset,USHORT len)
{
    BYTE __based(pBuffer) *pBased = 0;
    UNICODE_STRING us;
    us.Length = len;
    us.MaximumLength = us.Length;
    us.Buffer = (PWSTR)(pBased + offset);
    printf("%s%wZ\n",Title,&us);
}

void EnumVolumeInstance(PCWSTR pszVolumeName)
{
    HRESULT hr;
    HANDLE hVolumeInstanceFind;
    INSTANCE_INFORMATION_CLASS InfoClass;
    PVOID pBuffer;

    if( g_wVersion >= _WIN32_WINNT_VISTA )
        InfoClass = InstanceAggregateStandardInformation;
    else
        InfoClass = InstanceFullInformation;

    hr = _FindFirst_VolumeInstance(pszVolumeName,InfoClass,
                (PVOID *)&pBuffer,&hVolumeInstanceFind);

    if( hr == S_OK )
    {
        do
        {
            if( g_wVersion >= _WIN32_WINNT_VISTA )
            {
                INSTANCE_AGGREGATE_STANDARD_INFORMATION *piasi 
                    = (INSTANCE_AGGREGATE_STANDARD_INFORMATION *)pBuffer;

                if( piasi->Flags == FLTFL_IASI_IS_MINIFILTER )
                {
                    print_string("\tInstance Name: ",piasi,
                        piasi->Type.MiniFilter.InstanceNameBufferOffset,
                        piasi->Type.MiniFilter.InstanceNameLength);

                    print_string("\tFilter Name  : ",piasi,
                        piasi->Type.MiniFilter.FilterNameBufferOffset,
                        piasi->Type.MiniFilter.FilterNameLength);

                    print_string("\tAltitude     : ",piasi,
                        piasi->Type.MiniFilter.AltitudeBufferOffset,
                        piasi->Type.MiniFilter.AltitudeLength);

                    printf("\n");
                }
                else if( piasi->Flags == FLTFL_IASI_IS_LEGACYFILTER )
                {
                    print_string("\tFilter Name  : ",piasi,
                        piasi->Type.LegacyFilter.FilterNameBufferOffset,
                        piasi->Type.LegacyFilter.FilterNameLength);

                    print_string("\tAltitude     : ",piasi,
                        piasi->Type.LegacyFilter.AltitudeBufferOffset,
                        piasi->Type.LegacyFilter.AltitudeLength);

                    printf("\n");
                }
            }
            else
            {
                INSTANCE_FULL_INFORMATION *pifi
                    = (INSTANCE_FULL_INFORMATION *)pBuffer;

                print_string("\tInstance Name: ",pifi,
                    pifi->InstanceNameBufferOffset,
                    pifi->InstanceNameLength);

                print_string("\tFilter Name  : ",pifi,
                    pifi->FilterNameBufferOffset,
                    pifi->FilterNameLength);

                print_string("\tAltitude     : ",pifi,
                    pifi->AltitudeBufferOffset,
                    pifi->AltitudeLength);

                printf("\n");
            }
            free(pBuffer);

            hr = _FindNext_VolumeInstance(hVolumeInstanceFind,
							InfoClass,(PVOID *)&pBuffer);
        }
        while( hr == S_OK );
    }
    else
    {
        printf("\tenum volume instance error: 0x%08X\n\n",hr);
    }
}

int __cdecl wmain(int /*argc*/, WCHAR* /*argv*/[])
{
    HRESULT hr;
    HANDLE hFilterFind;
    DWORD BytesReturned;

    WORD wVersion = LOWORD(GetVersion());
    g_wVersion = MAKEWORD(HIBYTE(wVersion),LOBYTE(wVersion));

    DWORD dwBufferSize = sizeof(FILTER_VOLUME_STANDARD_INFORMATION)
						 + (sizeof(WCHAR) * _VOLUME_NAME_LENGTH);
    FILTER_VOLUME_STANDARD_INFORMATION *lpBuffer =
						 (FILTER_VOLUME_STANDARD_INFORMATION *)malloc(dwBufferSize);
    if( lpBuffer == NULL )
        return -1;

    hr = FilterVolumeFindFirst(
                FilterVolumeStandardInformation,
                lpBuffer,
                dwBufferSize,
                &BytesReturned,
                &hFilterFind
                ); 

    if( hr == S_OK )
    {
        do
        {
            WCHAR sz[_VOLUME_NAME_LENGTH+1];
            WCHAR szDosDrive[MAX_PATH];

            memcpy(sz,lpBuffer->FilterVolumeName,lpBuffer->FilterVolumeNameLength);
            sz[ lpBuffer->FilterVolumeNameLength/sizeof(WCHAR) ] = UNICODE_NULL;

            if( FilterGetDosName(sz,szDosDrive,MAX_PATH) != S_OK )
            {
                szDosDrive[0] = 0;
            }

            if( szDosDrive[0] != L'\0' )
                printf("%S (%s:)\n",sz,szDosDrive);
            else
                printf("%S\n",sz);

            EnumVolumeInstance(sz);

            hr = FilterVolumeFindNext(
                        hFilterFind,
                        FilterVolumeStandardInformation,
                        lpBuffer,
                        dwBufferSize,
                        &BytesReturned
                        ); 
        }
        while( hr == S_OK );
    }
    else
    {
        printf("error: 0x%08X\n",hr);
    }

    free(lpBuffer);

    return 0;
}
