public interface DbConnection {
    void insert(float co2, float humidity, float temperature, int roomId, float noise);
}
