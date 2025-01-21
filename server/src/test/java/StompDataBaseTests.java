
import org.junit.jupiter.api.Test;

import bgu.spl.net.impl.stomp.StompDataBase;

import static org.junit.jupiter.api.Assertions.*;

import java.util.List;
    public class StompDataBaseTests {
        @Test
        public void testUserPasswordRetrieval() {
            StompDataBase db = StompDataBase.getInstance();
            db.addOrUpdateUser("user1", "pass1", 1);
            db.addOrUpdateUser("user2", "pass2", 2);
            assertEquals(db.getPasswordForUser("user1"), "pass1");
            assertEquals(db.getPasswordForUser("user2"), "pass2");



        }
    
}
