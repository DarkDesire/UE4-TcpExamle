// Copyright 2016 Eldar Nezametdinov

#pragma once

#include "Networking.h" 
#include <string>

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyNetworkFunction.generated.h"

/**
 * 
 */
UCLASS()
class TCPEXAMPLE_API UMyNetworkFunction : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
  public:

  UFUNCTION( BlueprintCallable, Category = "SOKETCOMM" ) static void socket_init();

  private:

  static std::string get_content();
  void time_check();
  void socket_listener();


};
