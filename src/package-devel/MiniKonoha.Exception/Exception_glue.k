import("Minikonoha.EventListener");

void OutOfBoundsException(Event e) {
	System.p("out of bounds exception!");
	// you can write the way for out of bounds exception
}

// you can write other exception callbacks here

EventHandler.setCallBack("OutOfBoundsException", OutOfBoundsException);
// you can list the callbacks here

HttpEventListener.start("127.0.0.1", 8080);
