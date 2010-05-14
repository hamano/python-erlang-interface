-module(pingpong).
-compile(export_all).

start() -> register(pingpong, spawn(?MODULE, loop, [])).
stop()  -> pingpong ! stop.

loop() ->
  receive
    stop ->
      io:fwrite("Stop: ~w~n", [self()]),
      exit(ok);
    {ping, Pid, Message} ->
      io:fwrite("Pid: ~w, Message:~s~n", [Pid, Message]),
      Pid ! {pong, self(), Message};
    Other ->
      io:fwrite("Other: ~w~n", [Other])
  end,
  loop().

ping(Arg) ->
  case Arg of
    ping -> pong;
    stop -> exit(ok);
    _ -> pang
  end.
