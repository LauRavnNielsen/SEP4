import java.sql.*;
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.LocalTime;

public class DbConnectionImpl implements DbConnection{
    static String connectionStr =
            "jdbc:sqlserver://sepfour.database.windows.net:1433;database=SEP4;user=superuser@sepfour;password=28052020Ul;encrypt=true;trustServerCertificate=false;hostNameInCertificate=*.database.windows.net;loginTimeout=30;";
    public DbConnectionImpl(){
    }

    @Override
    public void insert(float co2, float humidity, float temperature, int roomId, float noise) {
        LocalDateTime date = LocalDateTime.now();
        String insertSql = "INSERT INTO [dbo].[Metrics] (Humidity, Temperature, Noise, CO2, LastUpdated, ProductID) " +
                "VALUES (?, ?, ?, ?, ?, ?)";
        ResultSet resultSet = null;
        System.out.println(date.toString());

        try
        {
            Connection connection = DriverManager.getConnection(connectionStr);
            PreparedStatement prepsInsertProduct = connection.prepareStatement(insertSql);

            prepsInsertProduct.setFloat     (1, humidity);
            prepsInsertProduct.setFloat     (2, temperature);
            prepsInsertProduct.setFloat     (3, noise);
            prepsInsertProduct.setFloat     (4, co2);
            prepsInsertProduct.setObject     (5, date);
            prepsInsertProduct.setInt       (6, roomId);

            prepsInsertProduct.execute();

        }
        // Handle any errors that may have occurred.
        catch (Exception e) {
            e.printStackTrace();
        }
    }
}