

// 0 = both, 1 = base, 2 = lid
mode = 1;

d1_length = 34.2;
d1_width = 25.6;

box_length = 70;
box_width = 50;
box_height = 10.5;

rcwl_length = 35.9;
rcwl_width = 17.3;

thickness = 2;
padding = 1.075 + (0.75/2);

xOffset = box_width/2 + 10;

if (mode == 0) {
    xOffset = box_width/2 + 10;
    box();
    templates();
    lid();
}
if (mode == 1) {
    xOffset = 0;
    box();
}

if (mode == 2) {
    xOffset = 0;
    lid();
}

module roundedBox(shrink, offset, height) {
    radius = 3;
    
    hull() {
        translate([-box_length/2+radius+shrink, -box_width/2+radius+shrink+offset, 0]) {
            cylinder(r1= 3, r2=3, h=height);
        }
        
        translate([-box_length/2+radius+shrink, box_width/2-radius-shrink+offset, 0]) {
            cylinder(r1= 3, r2=3, h=height);
        }
        
        translate([box_length/2-radius-shrink, -box_width/2+radius+shrink+offset, 0]) {
            cylinder(r1= 3, r2=3, h=height);
        }
        
        translate([box_length/2-radius-shrink, box_width/2-radius-shrink+offset, 0]) {
            cylinder(r1= 3, r2=3, h=height);
        }
    }
}

module box() {
    radius = 3;
    z = 2;
    z2 = box_height;
    
    difference() {
        
        roundedBox(0, xOffset, box_height);
        
        // Hollow out
        translate([0,0,2]) {
            roundedBox(2, xOffset, box_height);
        }
        
        // lip
        difference() {
            translate([0,0,8.5]) {
                roundedBox(1, xOffset, box_height);
            }
        }
       
       // usb lip 
        translate([-33, 35, 20])
                cube([2, 27,30], center=true);

//         // Usb Hole
        translate([-box_length/2-3, -5+xOffset, 5])
            cube([10, 10, 7]);   
     }
     
    frontRiser();
    leftRiser();
    rightRiser();
     ledHolder();
     clips();
}

module clips() {
    clip(-8, 12.1);
    rotate([0,0,180])
        clip(8, -22.9-xOffset);

    rotate([0,0,90])
        clip(23, -14.1-xOffset);
    rotate([0,0,90])
        clip(47, -14.1-xOffset);

    rotate([0,0,-90])
        clip(-47, 30-xOffset);
    rotate([0,0,-90])
        clip(-23, 30-xOffset);

    translate([13,xOffset-15,0]) {
        cube([2,6,6]);
    }

    translate([29,xOffset-15,0]) {
        cube([2,6,6]);
    }

    translate([13,xOffset+9,0]) {
        cube([2,6,6]);
    }

    translate([29,xOffset+9,0]) {
        cube([2,6,6]);
    }
}

module lid() {

    difference() {
        
        roundedBox(0, -xOffset, , 4);
        
        translate([0,0,2]) {
            roundedBox(2, -xOffset, 4);
        }
        
        translate([0,0,7]) {
            roundedBox(1, xOffset);
        }

         // LED Hole
       translate([6,0-xOffset, -1])
            cylinder(r1=1, r2=2.5, h=3);  
         
         
          // Usb Hole
       translate([-box_length/2-5, -5-xOffset, 3])
            cube([10, 10, 10]);   
         }
         
         difference() {
        
        roundedBox(1, -xOffset, , 6);
        
        translate([0,0,2]) {
            roundedBox(2, -xOffset, 6);
        }
        
        translate([0,0,7]) {
            roundedBox(1, xOffset);
        }
        
        // LED Hole
       translate([6,0-xOffset, -1])
            cylinder(r1=1, r2=2.5, h=3);  
         
         
          // Usb Hole
       translate([-box_length/2-5, -13.5-xOffset, 3])
            cube([10, 27, 10]);   
       }
}


module templates() {
    d1();
    rcwl();
    led();
}


module d1() {
    translate([-18,0+xOffset,6])
    cube([d1_length, d1_width, 2], center=true);
}

module frontRiser() {
    translate([-(box_length/2)+3,0+xOffset,3])
    cube([3,10,4], center=true);
}


module leftRiser() {
    translate([-8,12.1+xOffset,3])
    cube([6,2,4], center=true);
}

module rightRiser() {
    translate([-8,-12.1+xOffset,3])
    cube([6,2,4], center=true);
}


module clip(x, y) {
    translate([x, y+2+xOffset, 4.75])
        cube([6, 2, 6], center=true);
    
    translate([x-3, y+xOffset, 7.5])
            prism(6,3,1);
}




module  poles(x) {
    translate([x, 0+xOffset, 5]) {
        translate([rcwl_width/2-padding, rcwl_length/2-padding,0]) {
            cylinder(h=5, r1=.5, r2=.5, center=true);
            translate([0,0,-2]) {
                cylinder(h=4, r1=1, r2=1, center=true);
            }
        }
        
        translate([-rcwl_width/2+padding, rcwl_length/2-padding,0]) {
            cylinder(h=5, r1=.5, r2=.5, center=true);
            translate([0,0,-2]) {
                cylinder(h=4, r1=1, r2=1, center=true);
            }
        }
        
        translate([rcwl_width/2-padding, -rcwl_length/2+padding,0]) {
            cylinder(h=5, r1=.5, r2=.5, center=true);
            translate([0,0,-2]) {
                cylinder(h=4, r1=1, r2=1, center=true);
            }
        }
        
        translate([-rcwl_width/2+padding, -rcwl_length/2+padding,0]) {
            cylinder(h=5, r1=.5, r2=.5, center=true);
            translate([0,0,-2]) {
                cylinder(h=4, r1=1, r2=1, center=true);
            }
        }
        
        translate([rcwl_width/2-padding, -rcwl_length/2+padding+15,0]) {
            cylinder(h=5, r1=.5, r2=.5, center=true);
            translate([0,0,-2]) {
                cylinder(h=4, r1=1, r2=1, center=true);
            }
        }
        
        translate([-rcwl_width/2+padding, -rcwl_length/2+padding+15,0]) {
            cylinder(h=5, r1=.5, r2=.5, center=true);
            translate([0,0,-2]) {
                cylinder(h=4, r1=1, r2=1, center=true);
            }
        }
    }
}

module led() {
    translate([6, xOffset, 3])
        cylinder(r1=2.5, r2=2.5, h=8.7);
}

module rcwl() {
    translate([22, 0+xOffset, 6]) {
        difference() {
            cube([rcwl_width, rcwl_length, 2], center=true);
            
            translate([rcwl_width/2-padding, rcwl_length/2-padding,-1])
                cylinder(h=10, r1=1, r2=1, center=true);
            
            translate([-rcwl_width/2+padding, rcwl_length/2-padding,-1])
                cylinder(h=10, r1=1, r2=1, center=true);
            
             translate([rcwl_width/2-padding, -rcwl_length/2+padding,-1])
                cylinder(h=10, r1=1, r2=1, center=true);
            
            translate([-rcwl_width/2+padding, -rcwl_length/2+padding,-1])
                cylinder(h=10, r1=1, r2=1, center=true);
        }
    }
}

module ledHolder() {
    translate([3, xOffset-8, 0]) {
        
        offset = 1.4;
        
       difference() {
            cube([6,4,5]);
           
           translate([offset, 0, 3])
           cube([0.6,15,2]);
           
           translate([offset+2.54, 0, 3])
           cube([0.6,15,2]);
        }
    }
    
    translate([3, xOffset+4, 0]) {
        
        offset = 1.4;
        
       difference() {
            cube([6,4,5]);
           
           translate([offset, 0, 3])
           cube([0.6,15,2]);
           
           translate([offset+2.54, 0, 3])
           cube([0.6,15,2]);
        }
    }
}

module prism(l, w, h){
       polyhedron(
               points=[[0,0,0], [l,0,0], [l,w,0], [0,w,0], [0,w,h], [l,w,h]],
               faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
        );
}