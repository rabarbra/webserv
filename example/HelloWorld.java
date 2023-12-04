import java.util.Map;

public class HelloWorld {
    public static void main(String[] args) {
        // Get the system environment variables
        Map<String, String> envVariables = System.getenv();

        // Iterate through the environment variables and print them
        for (Map.Entry<String, String> entry : envVariables.entrySet()) {
            System.out.println(entry.getKey() + " = " + entry.getValue());
        }
    }
}

