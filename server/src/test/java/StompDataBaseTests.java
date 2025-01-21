
import org.junit.jupiter.api.Test;

import bgu.spl.net.impl.stomp.StompDataBase;
import bgu.spl.net.impl.stomp.StompFrame;
import bgu.spl.net.impl.stomp.StompMessagingProtocolImpl;
import bgu.spl.net.srv.Connections;

import static org.junit.jupiter.api.Assertions.*;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
    public class StompDataBaseTests {
        //צריך לבדוק טרמינייט
        @Test
        public void testUserPasswordRetrieval() {
            StompDataBase db = StompDataBase.getInstance();
            db.addOrUpdateUser("user1", "pass1", 1);
            db.addOrUpdateUser("user2", "pass2", 2);
            assertEquals(db.getPasswordForUser("user1"), "pass1");
            assertEquals(db.getPasswordForUser("user2"), "pass2");
        }
       
        @Test
        public void testConnectHendelSuccess() {
        // Create the protocol instance
        StompMessagingProtocolImpl protocol = new StompMessagingProtocolImpl();
        
        StompFrame connectFrame = new StompFrame("CONNECT");
        connectFrame.setHeadersByPart("accept-version", "1.2");
        connectFrame.setHeadersByPart("host", "stomp.cs.bgu.ac.il");
        connectFrame.setHeadersByPart("login", "meni");
        connectFrame.setHeadersByPart("passcode", "films");

        StompFrame respond = protocol.connectHendel(connectFrame);

        assertEquals("CONNECTED", respond.getCommend());
        assertEquals("1.2", respond.getHeaderValue("version"));
          
        StompDataBase db = StompDataBase.getInstance();
        assertEquals(db.getPasswordForUser("meni"), "films");
        assertEquals(db.getPasswordForUser("meni"), "films");

        }
        @Test
        public void testConnectHendeError1(){
            StompMessagingProtocolImpl protocol = new StompMessagingProtocolImpl();
        
            StompFrame connectFrame = new StompFrame("CONNECT");
            connectFrame.setHeadersByPart("accept-version", "1.2");
            connectFrame.setHeadersByPart("host", "stomp.cs.bgu.ac.il");
            connectFrame.setHeadersByPart("login", "meni");
            connectFrame.setHeadersByPart("passcode", "films");
    
            StompFrame respond = protocol.connectHendel(connectFrame);
      
        }
        
    
}
