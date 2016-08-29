// Copyright 2016 Eldar Nezametdinov

#include "TcpExample.h"
#include "MyNetworkFunction.h"

#include <sstream>

int state = 1;

std::string UMyNetworkFunction::get_content()
{
  FIPv4Address ip;
  const std::string server_ip = "127.0.0.1";
  int port = 8080;
  if ( !FIPv4Address::Parse( UTF8_TO_TCHAR( server_ip.c_str() ), ip ) ) {
    UE_LOG( LogTemp, Warning, TEXT( "FIPv4Address::Parse error" ) );
    return "FIPv4Address::Parse error";
  }

  TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->CreateInternetAddr();
  Addr->SetIp( ip.Value );
  Addr->SetPort( port );//*/

  FSocket* Socket = ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->CreateSocket( NAME_Stream, TEXT( "default" ), false );
  if ( !Socket ) {
    UE_LOG( LogTemp, Warning, TEXT( "ERROR create socket" ) );
    return "ERROR create socket";
  }

  if ( !Socket->Connect( *Addr ) ) {
    Socket->Close();
    ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->DestroySocket( Socket );
    UE_LOG( LogTemp, Warning, TEXT( "ERROR connect to socket" ) );
    return "ERROR connect to socket";
  }

  std::string message_str = "Hello World!";//
  int32 BytesSent;
  if ( !Socket->Send( (uint8*)message_str.c_str(), message_str.size(), BytesSent ) || BytesSent != message_str.size() ) {
    Socket->Close();
    ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->DestroySocket( Socket );
    UE_LOG( LogTemp, Warning, TEXT( "ERROR send string" ) );
    return "ERROR send string";
  }
  else {
    UE_LOG( LogTemp, Warning, TEXT( "OK send string" ) );
  }

  while ( true ) {
    if ( !Socket->Wait( ESocketWaitConditions::WaitForRead, FTimespan( 0, 0, 30 ) ) )//timeout_s)))
    {
      Socket->Close();
      ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->DestroySocket( Socket );
      UE_LOG( LogTemp, Warning, TEXT( "TIMEOUT there is no input data during 30sek" ) );
      return "TIMEOUT there is no input data during 30sek)";
    }
    uint8 Data[10000];
    int32 BytesRead;
    if ( !Socket->Recv( Data, sizeof( Data ), BytesRead ) ) {
      Socket->Close();
      ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->DestroySocket( Socket );
      UE_LOG( LogTemp, Warning, TEXT( "ERROR recv" ) );
      return "ERROR recv";
    }
    else {
      UE_LOG( LogTemp, Warning, TEXT( "OK recv" ) );
    }

    std::string contOrig;

    GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, TEXT( "OK recv" ) );

    std::string *cont = &contOrig;
    &cont->append( (char *)Data, BytesRead );
    {
      std::string sep_tag = "\r\n\r\n";
      int32 sep_pos = cont->find( sep_tag );
      if ( sep_pos != std::string::npos ) {
        std::string len_tag = "Content-Length: ";
        int32 len_pos = cont->find( len_tag );
        if ( len_pos != std::string::npos ) {
          int32 pos = cont->find( "\r\n", len_pos );
          if ( pos != std::string::npos ) {
            std::string len_str = cont->substr( len_pos + len_tag.size(), pos - len_pos - len_tag.size() );
            std::stringstream ss;
            ss << len_str;
            int32 len = 0;
            ss >> len;
            if ( len < 0 ) {
              Socket->Close();
              ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->DestroySocket( Socket );
              return "bad len";
            }
            if ( sep_pos + sep_tag.size() + len == cont->size() ) {
              *cont = cont->substr( sep_pos + sep_tag.size() );
              break;
            }
          }
        }
      }
    }

    FString boom( contOrig.c_str() );
    UE_LOG( LogTemp, Warning, TEXT( "RECEIVED String: %s" ), *boom );

    if ( !BytesRead ) {
      Socket->Close();
      ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->DestroySocket( Socket );
      return "connection closed";
    }
  }
  //*/

  Socket->Close();
  ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->DestroySocket( Socket );
  UE_LOG( LogTemp, Warning, TEXT( "OK" ) );
  return "ok";
}

void UMyNetworkFunction::socket_listener()
{
  UE_LOG( LogTemp, Warning, TEXT( "__SOCKET_LISTENER__" ) );

}

void UMyNetworkFunction::socket_init()
{
  UE_LOG( LogTemp, Warning, TEXT( "GETCONTSTART" ) );
  get_content();
  UE_LOG( LogTemp, Warning, TEXT( "GETCONTFINISH" ) );
}

void UMyNetworkFunction::time_check()
{
  UE_LOG( LogTemp, Warning, TEXT( "This is TIMER" ) );
}
