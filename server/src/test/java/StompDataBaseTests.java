
import org.junit.jupiter.api.Test;

import bgu.spl.net.impl.stomp.StompDataBase;
import bgu.spl.net.impl.stomp.StompFrame;
import bgu.spl.net.impl.stomp.StompMessagingProtocolImpl;

import static org.junit.jupiter.api.Assertions.*;

    public class StompDataBaseTests {
        //צריך לבדוק טרמינייט
        // @Test
        // public void testUserPasswordRetrieval() {
        //     StompDataBase db = StompDataBase.getInstance();
        //     db.addOrUpdateUser("user1", "pass1", 0);
        //     db.addOrUpdateUser("user2", "pass2", 1);
        //     assertEquals(db.getPasswordForUser("user1"), "pass1");
        //     assertEquals(db.getPasswordForUser("user2"), "pass2");

        // }
       
        @Test
        public void testConnectHendelSuccess() {
        // Create the protocol instance
        StompDataBase db = StompDataBase.getInstance();
        StompMessagingProtocolImpl protocol = new StompMessagingProtocolImpl();
        
        StompFrame connectFrame = new StompFrame("CONNECT");
        connectFrame.setHeadersByPart("accept-version", "1.2");
        connectFrame.setHeadersByPart("host", "stomp.cs.bgu.ac.il");
        connectFrame.setHeadersByPart("login", "nono");
        connectFrame.setHeadersByPart("passcode", "mimi");

        StompFrame respond = protocol.connectHendel(connectFrame);
        StompFrame respond2 = protocol.connectHendel(connectFrame);



        assertEquals("CONNECTED", respond.getCommend());
        assertEquals("1.2", respond.getHeaderValue("version"));
          
        assertEquals(db.getPasswordForUser("nono"), "mimi");

        }
        @Test
        public void testConnectHendeError1(){
            StompMessagingProtocolImpl protocol = new StompMessagingProtocolImpl();
        
            StompFrame connectFrame = new StompFrame("CONNECT");
            connectFrame.setHeadersByPart("accept-version", null);
            connectFrame.setHeadersByPart("host", "stomp.cs.bgu.ac.il");
            connectFrame.setHeadersByPart("login", "meni");
            connectFrame.setHeadersByPart("passcode", "films");
    
            StompFrame respond = protocol.connectHendel(connectFrame);
            assertEquals("missing header", respond.getHeaderValue("message"));
        }

        @Test
        public void testConnectHendeError2(){
            StompMessagingProtocolImpl protocol = new StompMessagingProtocolImpl();
        
            StompFrame connectFrame = new StompFrame("CONNECT");
            connectFrame.setHeadersByPart("accept-version", "1");
            connectFrame.setHeadersByPart("host", "stomp.cs.bgu.ac.il");
            connectFrame.setHeadersByPart("login", "meni");
            connectFrame.setHeadersByPart("passcode", "films");
    
            StompFrame respond = protocol.connectHendel(connectFrame);
            assertEquals("Unsupported version", respond.getHeaderValue("message"));
        }

        @Test
        public void testConnectHendeError3(){
            StompMessagingProtocolImpl protocol = new StompMessagingProtocolImpl();
        
            StompFrame connectFrame = new StompFrame("CONNECT");
            connectFrame.setHeadersByPart("accept-version", "1.2");
            connectFrame.setHeadersByPart("host", "stomp.cs.bgu.ac.il");
            connectFrame.setHeadersByPart("login", "meni");
            connectFrame.setHeadersByPart("passcode", "films");
            protocol.connectHendel(connectFrame);
            StompFrame respond = protocol.connectHendel(connectFrame);
            assertEquals("User already logged in", respond.getHeaderValue("message"));
        }

        @Test
        public void testConnectHendeError4(){
            StompMessagingProtocolImpl protocol = new StompMessagingProtocolImpl();
            StompDataBase db = StompDataBase.getInstance();

            StompFrame connectFrame = new StompFrame("CONNECT");
            connectFrame.setHeadersByPart("accept-version", "1.2");
            connectFrame.setHeadersByPart("host", "stomp.cs.bgu.ac.il");
            connectFrame.setHeadersByPart("login", "meni");
            connectFrame.setHeadersByPart("passcode", "films");
            
            StompFrame disconnectFrame = new StompFrame("DISCONNECT");
            disconnectFrame.setHeadersByPart("receipt", "1");
            disconnectFrame.setFrameBody(null);
            
            protocol.connectHendel(connectFrame);
            protocol.disconnectHendel(disconnectFrame);
            
            assertFalse(db.getConnectedUsers(1));

            StompFrame respond = protocol.connectHendel(connectFrame);
            assertEquals(db.getPasswordForUser("meni"), "films");
            assertEquals("CONNECTED", respond.getCommend());

        }
        @Test
    public void testSendhendelSucsseful(){
        StompMessagingProtocolImpl protocol = new StompMessagingProtocolImpl();
        StompDataBase db = StompDataBase.getInstance();
        
         StompFrame sendFrame = new StompFrame("SEND");
         sendFrame.setHeadersByPart("destination", "/police");
         sendFrame.setHeadersByPart("receipt", "12");
         sendFrame.setFrameBody("Hello topic police");
         
         StompFrame subscribeFrame = new StompFrame("SUBSCRIBE");
         subscribeFrame.setHeadersByPart("destination", "police");
         subscribeFrame.setHeadersByPart("id", "78");
         subscribeFrame.setHeadersByPart("receipt", "12");
         subscribeFrame.setFrameBody(null);
        
         StompFrame respond1 = protocol.subscribeHendel(subscribeFrame);

         StompFrame respond2 = protocol.sendHendel(sendFrame);

         assertEquals("RECEIPT", respond1.getCommend());
         assertEquals("12", respond1.getHeaderValue("receipt - id"));
         assertEquals(db.getChannel(0, 78), "police");
         assertEquals("MESSAGE", respond2.getCommend());
         assertEquals("police", respond2.getHeaderValue("destination"));

    }
    @Test
    public void testSendhendelError1(){
        StompMessagingProtocolImpl protocol = new StompMessagingProtocolImpl();
        StompDataBase db = StompDataBase.getInstance();
        StompFrame sendFrame = new StompFrame("SEND");
         sendFrame.setHeadersByPart("destination", null);
         sendFrame.setHeadersByPart("receipt", "12");
         sendFrame.setFrameBody("Hello topic police");
         
         StompFrame subscribeFrame = new StompFrame("SUBSCRIBE");
         subscribeFrame.setHeadersByPart("destination", "police");
         subscribeFrame.setHeadersByPart("id", "78");
         subscribeFrame.setHeadersByPart("receipt", "12");
         subscribeFrame.setFrameBody(null);
        
         protocol.subscribeHendel(subscribeFrame);

    }   
        
    @Test
    public void testDisconnect() {
        StompMessagingProtocolImpl protocol = new StompMessagingProtocolImpl();
        
        StompFrame connectFrame = new StompFrame("CONNECT");
        connectFrame.setHeadersByPart("accept-version", "1.2");
        connectFrame.setHeadersByPart("host", "stomp.cs.bgu.ac.il");
        connectFrame.setHeadersByPart("login", "meni");
        connectFrame.setHeadersByPart("passcode", "films");

        protocol.connectHendel(connectFrame);

        StompDataBase db = StompDataBase.getInstance();

        // יצירת frame של DISCONNECT
        StompFrame disconnectFrame = new StompFrame("DISCONNECT");
        disconnectFrame.setHeaders("receipt:0");

        // שלב 3: ביצוע ההתנתקות
        StompFrame response = protocol.disconnectHendel(disconnectFrame);

        // שלב 4: וידוא מחיקת נתונים
        assertFalse(db.isConnectedUser(0)); // המשתמש הוסר מהמחוברים

        // שלב 5: בדיקת תגובת RECEIPT
        assertNotNull(response);
        assertEquals("RECEIPT", response.getCommend());
        assertEquals("0", response.getHeaderValue("receipt-id"));

    }
    
}
