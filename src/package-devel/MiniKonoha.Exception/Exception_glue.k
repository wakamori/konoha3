import("Minikonoha.EventListener");

void OutOfBoundsException(Event e) {
	System.p("out of bounds exception!");
	// you can write the way for out of bounds exception
}

EventHandler.setCallBack("OutOfBoundsException", OutOfBoundsException);

HttpEventListener.start("0.0.0.0", 8080);
