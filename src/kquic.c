#include "../include/kaibash.h"


/*
This function will be used to send packets via QUIC both ways C->S and S->C
*/
void server_send(_In_ HQUIC Stream) {
    void *send_buffer_raw_ptr = malloc(sizeof(QUIC_BUFFER) + SendBufferLength);

    if (!send_buffer_raw_ptr) {
        Sasprintf(stderr, "SendBuffer allocation failed");
        MsQuic->StreamShutdown(Stream, QUIC_STREAM_SHUTDOWN_FLAG_ABORT, 0);

        return;
    }

    QUIC_BUFFER *send_buffer_ptr = (QUIC_BUFFER*)send_buffer_raw_ptr;
    send_buffer_ptr->Buffer = (uint8_t*)send_buffer_raw_ptr + sizeof(QUIC_BUFFER);
    send_buffer_ptr->Length = SendBufferLength;

    Sasprintf(stdout, "[strm][%p] Sending data..\n", Stream);

    QUIC_STATUS status;

    if (QUIC_FAILED(status = MsQuic->StreamSend(Stream, send_buffer_ptr, 1, QUIC_SEND_FLAG_FIN, send_buffer_ptr))) {
        Sasprintf(stderr, "StreamSend failed 0x%x\n", status);
        free(send_buffer_raw_ptr);
        MsQuic->StreamShutdown(Stream, QUIC_STREAM_SHUTDOWN_FLAG_ABORT, 0);
    }

}

/*
Server's callback for streamevent
*/
_IRQL_requires_max_(DISPATCH_LEVEL)
_Function_class_(QUIC_STREAM_CALLBACK)
QUIC_STATUS
QUIC_API
server_stream_callback(
    _In_ HQUIC Stream,
    _In_opt_ void* Context,
    _Inout_ QUIC_STREAM_EVENT* Event
) {
    UNREFERENCED_PARAMETER(Context);

    switch (Event->Type) {
        case QUIC_STREAM_EVENT_SEND_COMPLETE:
            //a previous StreamSend was completed
            //and the contenxt is being returned to the app
            free(Event->SEND_COMPLETE.ClientContext);
            Sasprintf(stdout, "[strm][%p] Data sent\n", Stream);
            break;
        case QUIC_STREAM_EVENT_RECEIVE:
            //data was received from the peer

            //TODO: Handle data receive

            Sasprintf(stdout, "[strm][%p] Data received\n", Stream);
            break;
        case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
            //peer shut down gracefully on the other side of the stream
            
            Sasprintf(stdout, "[strm][%p] Peer shut down\n", Stream);
            server_send(Stream);
            break;
        case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
            //peer aborted its send direction
            
            Sasprintf(stdout, "[strm][%p] Peer aborted\n", Stream);
            MsQuic->StreamShutdown(Stream, QUIC_STREAM_SHUTDOWN_FLAG_ABORT, 0);
            break;
        case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
            //both sides shut down. Now MsQuic can college away the stream.

            Sasprintf(stdout, "[strm][%p] All done\n", Stream);
            MsQuic->StreamClose(Stream);
            break;
        default:
            break;

    }

    return QUIC_STATUS_SUCCESS;
}


/* 
Server callback for connections
*/
_IRQL_requires_max_(DISPATCH_LEVEL)
_Function_class_(QUIC_CONNECTION_CALLBACK)
QUIC_STATUS
QUIC_API
server_connection_callback(
    _In_ HQUIC Connection,
    _In_opt_ void* Context,
    _Inout_ QUIC_CONNECTION_EVENT* Event
) {
    UNREFERENCED_PARAMETER(Context);
    
    switch (Event->Type) {
        case QUIC_CONNECTION_EVENT_CONNECTED:
            //handshake completed
            Sasprintf(stdout, "[conn][%p] Connected\n", Connection);
            MsQuic->ConnectionSendResumptionTicket(Connection, QUIC_SEND_RESUMPTION_FLAG_NONE, 0, NULL);
            break;
        case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
            //the connection has been shut down by transport. Generally, this is
            //the expected way for the connection to shut down with its protocol when idle

            if (Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status == QUIC_STATUS_CONNECTION_IDLE) {
                Sasprintf(stdout, "[conn][%p] Shutdown succesfully on idle.\n", Connection);
            } else {
                Sasprintf(stdout, "[conn][%p] Shutdown by transport.\n", Connection);
            }

            break;
        case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
                Sasprintf("[conn][%p] Shut down by peer, 0x%llu\n", Connection, (unsigned long long)Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode);
                break;
        case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
            //The connection has been servied its task and completed.
            //ready for cleanup

            Sasprintf(stdout, "[conn][%p] Shutdown succesfully on idle.\n", Connection);
            MsQuic->ConnectionClose(Connection);
            break;
        case QUIC_CONNECTION_EVENT_PEER_STREAM_STARTED:
            //The peer has started/create a new stream. The app must set the callback boefore returning.

            Sasprintf(stdout, "[strm][%p] Peer started\n.", Event->PEER_STREAM_STARTED.Stream);
            MsQuic->SetCallbackHandler(Event->PEER_STREAM_STARTED.Stream, (void*)server_stream_callback, NULL);
            break;
        case QUIC_CONNECTION_EVENT_RESUMED:
            //the connection succeeded doing a TLS resumption of the previous session.
            Sasprintf(stdout, "[strm][%p] Connection resumed\n.", Connection);
            break;
        default:
            break;

    }

    return QUIC_STATUS_SUCCESS;
}


/* 
Set callback
*/
_IRQL_requires_max_(PASSIVE_LEVEL)
_Function_class_(QUIC_LISTENER_CALLBACK)
QUIC_STATUS
QUIC_API
server_listenr_callback(
    _In_ HQUIC Listener,
    _In_opt_ void* Context,
    _Inout_ QUIC_LISTENER_EVENT* Event
) {
    UNREFERENCED_PARAMETER(Listener);
    UNREFERENCED_PARAMETER(Context);
    QUIC_STATUS Status = QUIC_STATUS_NOT_SUPPORTED;
    switch (Event->Type) {
    case QUIC_LISTENER_EVENT_NEW_CONNECTION:
        //
        // A new connection is being attempted by a client. For the handshake to
        // proceed, the server must provide a configuration for QUIC to use. The
        // app MUST set the callback handler before returning.
        //
        MsQuic->SetCallbackHandler(Event->NEW_CONNECTION.Connection, (void*)server_connection_callback, NULL);
        Status = MsQuic->ConnectionSetConfiguration(Event->NEW_CONNECTION.Connection, Configuration);
        break;
    default:
        break;
    }
    return Status;
}

void
run_server(
    _In_ int argc,
    _In_reads_(argc) _Null_terminated_ char* argv[]
) {
    QUIC_STATUS status;
    HQUIC listener = NULL;

    //configure the address used for the listener to listen to all IP
    //addresses and the given UDP port
    QUIC_ADDR address = {0};
    QuicAddrSetFamily(&address, QUIC_ADDRESS_FAMILY_UNSPEC);
    QuicAddrSetPut(&address, UdpPort);

    //load the configuration
    

}