
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.WebSocket;
import java.nio.ByteBuffer;
import java.util.concurrent.CompletionStage;
import java.util.concurrent.CompletableFuture;

public class WebSocketClient implements WebSocket.Listener {
    private WebSocket server = null;
    private String TOKEN = "wss://iotnet.teracom.dk/app?token=vnoSwAAAABFpb3RuZXQudGVyYWNvbS5ka_PUdsF7f2NeS2qMCNPTXQg=";
    //TODO:  hree
     private final DbConnection database  = new DbConnectionImpl();

    // Send down-link message to device
    // Must be in Json format according to https://github.com/ihavn/IoT_Semester_project/blob/master/LORA_NETWORK_SERVER.md
    public void sendDownLink(String jsonTelegram) {
        server.sendText(jsonTelegram,true);
    }

    // E.g. url: "wss://iotnet.teracom.dk/app?token=??????????????????????????????????????????????="
    // Substitute ????????????????? with the token you have been given
    public WebSocketClient() {
        HttpClient client = HttpClient.newHttpClient();
        CompletableFuture<WebSocket> ws = client.newWebSocketBuilder()
                .buildAsync(URI.create(TOKEN), this);

        server = ws.join();
    }

    //onOpen()
    public void onOpen(WebSocket webSocket) {
        // This WebSocket will invoke onText, onBinary, onPing, onPong or onClose methods on the associated listener (i.e. receive methods) up to n more times
        webSocket.request(1);
        System.out.println("WebSocket Listener has been opened for requests.");
    }

    //onError()
    public void onError​(WebSocket webSocket, Throwable error) {
        System.out.println("A " + error.getCause() + " exception was thrown.");
        System.out.println("Message: " + error.getLocalizedMessage());
        webSocket.abort();
    };
    //onClose()
    public CompletionStage<?> onClose(WebSocket webSocket, int statusCode, String reason) {
        System.out.println("WebSocket closed!");
        System.out.println("Status:" + statusCode + " Reason: " + reason);
        return new CompletableFuture().completedFuture("onClose() completed.").thenAccept(System.out::println);
    };
    //onPing()
    public CompletionStage<?> onPing​(WebSocket webSocket, ByteBuffer message) {
        webSocket.request(1);
        System.out.println("Ping: Client ---> Server");
        System.out.println(message.asCharBuffer().toString());
        return new CompletableFuture().completedFuture("Ping completed.").thenAccept(System.out::println);
    };
    //onPong()
    public CompletionStage<?> onPong​(WebSocket webSocket, ByteBuffer message) {
        webSocket.request(1);
        System.out.println("Pong: Client ---> Server");
        System.out.println(message.asCharBuffer().toString());
        return new CompletableFuture().completedFuture("Pong completed.").thenAccept(System.out::println);
    };
    //onText()
    public CompletionStage<?> onText​(WebSocket webSocket, CharSequence data, boolean last) {
        webSocket.request(1);

        String dataString = data.toString();
        var parser = new JSONParser();
        JSONObject json;
        String hexData =  "";
        String cmd = "";
        try {
            json  = (JSONObject) parser.parse(dataString);
            hexData = (String) json.get("data");
            cmd = (String) json.get("cmd");
        } catch (ParseException e) {
            e.printStackTrace();
        }

        if (cmd.equalsIgnoreCase("rx")) {
            String[] hexMeasurement = hexData.split("(?<=\\G....)");

            int intHumidity = Integer.parseInt(hexMeasurement[0], 16);
            int intTemperature = Integer.parseInt(hexMeasurement[1], 16);
            int intCO2 = Integer.parseInt(hexMeasurement[2], 16);
            int intNoise = Integer.parseInt(hexMeasurement[3], 16);


            float floatHumidity = ((float)intHumidity);
            float floatTemperature = ((float)intTemperature);
            float floatCO2 = ((float)intCO2);
            float floatNoise = ((float)intNoise);

            database.insert(floatCO2, floatHumidity, floatTemperature, 55555555, floatNoise);
            System.out.println("Temperature: " + floatTemperature + "\nHumidity: " + floatHumidity + "\nCO2: "  + floatCO2 + "\nSound " + floatNoise + "\n");
            return new CompletableFuture().completedFuture("onText() completed.").thenAccept(System.out::println);
        }

        return new CompletableFuture().completedFuture("onText() not complete due to wrong cmd identifier.").thenAccept(System.out::println);
    };
}